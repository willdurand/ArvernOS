#include "frame.h"
#include <inttypes.h>
#include <mmu/logging.h>
#include <panic.h>
#include <string.h>

opt_uint64_t read_mmap(uint64_t request);
opt_uint64_t frame_number_to_request(frame_number_t frame_number);

// These variables can be accessed by other parts of the kernel and we do that
// in `paging_init()` to identity map the bitmap for allocated frames.
uint32_t frames_for_bitmap = 0;
bitmap_t* allocated_frames = NULL;

static const memory_region_t* memory_regions = NULL;
static uint32_t memory_regions_count = 0;
static uint64_t max_allocatable_frames = 0;
static uint64_t first_request = 0;

static uint64_t page_align_up(uint64_t address)
{
  uint64_t last_byte = address + PAGE_SIZE - 1;

  return frame_start_address(frame_containing_address(last_byte));
}

static bool range_is_available(uint64_t addr, uint64_t size)
{
  for (uint32_t i = 0; i < memory_regions_count; i++) {
    const memory_region_t* entry = &memory_regions[i];
    if (entry->type != MEMORY_AVAILABLE) {
      continue;
    }

    if (addr >= entry->addr && addr + size <= entry->addr + entry->len) {
      return true;
    }
  }

  return false;
}

void frame_init()
{
  INFO("%s", "mmu: initialize frame allocator");

  memory_reserved_areas_t reserved_areas = memory_get_reserved_areas();

  memory_regions_t regions = memory_get_regions();

  _frame_init(&reserved_areas, regions.regions, regions.count);

  DEBUG("kernel_start=%p kernel_end=%p max_allocatable_frames=%" PRIu64
        " allocated_frames=%p used_count=%" PRIu64 " first_request=%" PRIu64,
        reserved_areas.kernel_start,
        reserved_areas.kernel_end,
        max_allocatable_frames,
        allocated_frames,
        frame_get_used_count(),
        first_request);
}

void _frame_init(memory_reserved_areas_t* reserved_areas,
                 const memory_region_t* regions,
                 uint32_t region_count)
{
  memory_regions = regions;
  memory_regions_count = region_count;

  uint64_t available_memory = 0;
  for (uint32_t i = 0; i < region_count; i++) {
    if (regions[i].type != MEMORY_AVAILABLE) {
      continue;
    }

    available_memory += regions[i].len;
  }

  max_allocatable_frames = available_memory / PAGE_SIZE;
  frames_for_bitmap =
    ((max_allocatable_frames / BITS_PER_WORD) * sizeof(bitmap_t)) / PAGE_SIZE +
    1;

  // The bitmap goes right after the kernel image, skipping the bootloader
  // areas that would otherwise collide with it.
  uint64_t bitmap_size = frames_for_bitmap * PAGE_SIZE;
  uint64_t bitmap_address = page_align_up(reserved_areas->kernel_end);

  bool moved = true;
  while (moved) {
    moved = false;
    for (uint32_t i = 0; i < reserved_areas->area_count; i++) {
      const memory_area_t* area = &reserved_areas->areas[i];
      if (area->end <= area->start) {
        continue;
      }

      if (bitmap_address < area->end &&
          area->start < bitmap_address + bitmap_size) {
        // Move past this area. The new location may overlap another area,
        // hence the loop until nothing moves anymore.
        bitmap_address = page_align_up(area->end);
        moved = true;
      }
    }
  }

  if (!range_is_available(bitmap_address, bitmap_size)) {
    PANIC("bitmap at %p (%" PRIu64 " bytes) does not sit in an available "
          "memory region",
          bitmap_address,
          bitmap_size);
  }

  MMU_DEBUG("bitmap: address=%p size=%" PRIu64 " (%" PRIu64 " pages)",
            bitmap_address,
            bitmap_size,
            bitmap_size / PAGE_SIZE);

#ifndef TEST_ENV
  allocated_frames = (bitmap_t*)bitmap_address;
  memset(allocated_frames, 0, bitmap_size);
#endif

  // Mark frames as used from address `0` to the end of the memory allocated
  // for the "allocated frames".
  frame_number_t end = frame_containing_address(bitmap_address + bitmap_size);
  for (frame_number_t frame = 0; frame < end; frame++) {
    // Frames in the memory map's holes are not allocatable, so there is no
    // request to mark for them.
    opt_uint64_t request = frame_number_to_request(frame);
    if (request.has_value) {
      bitmap_set(allocated_frames, request.value);
      // Requests are ordered like frames: the last marked one is followed by
      // the first request the allocator may consider.
      first_request = request.value + 1;
    }
  }

  // Mark the frames used by the bootloader areas (e.g. the init ramdisk),
  // which typically sit in usable RAM.
  for (uint32_t i = 0; i < reserved_areas->area_count; i++) {
    const memory_area_t* area = &reserved_areas->areas[i];
    MMU_DEBUG("reserved area #%u: start=%p end=%p flags=%#x",
              i,
              area->start,
              area->end,
              area->flags);

    if (!(area->flags & MEMORY_AREA_NO_ALLOC) || area->end <= area->start) {
      continue;
    }

    for (frame_number_t frame = frame_containing_address(area->start);
         frame <= frame_containing_address(area->end - 1);
         frame++) {
      // Frames outside of the available regions are not allocatable, so there
      // is nothing to mark for them.
      opt_uint64_t request = frame_number_to_request(frame);
      if (request.has_value) {
        bitmap_set(allocated_frames, request.value);
      }
    }
  }
}

void _frame_set_bitmap(bitmap_t* bitmap)
{
  allocated_frames = bitmap;
}

opt_uint64_t frame_allocate()
{
  uint64_t request = 0;
  for (uint64_t i = first_request; i < max_allocatable_frames; i++) {
    if (bitmap_get(allocated_frames, i) == false) {
      request = i;
      break;
    }
  }

  if (request == 0) {
    // Should we PANIC instead?
    WARN("%s", "no more allocatable frames");
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  opt_uint64_t frame = read_mmap(request);

  if (frame.has_value) {
    MMU_DEBUG(
      "allocated frame: addr=%p request=%" PRIu64, frame.value, request);
    bitmap_set(allocated_frames, request);
  } else {
    MMU_DEBUG("could not allocate frame: request=%" PRIu64, request);
  }

  return frame;
}

void frame_deallocate(frame_number_t frame_number)
{
  opt_uint64_t request = frame_number_to_request(frame_number);
  MMU_DEBUG("deallocating frame=%" PRIu64 " request=%" PRIu64,
            frame_number,
            request.value);

  if (!request.has_value) {
    DEBUG("failed to deallocate frame=%" PRIu64
          " because it is not in an available memory region",
          frame_number);
    return;
  }

  bitmap_clear(allocated_frames, request.value);
}

frame_number_t frame_containing_address(uint64_t physical_address)
{
  return physical_address / PAGE_SIZE;
}

uint64_t frame_start_address(frame_number_t frame_number)
{
  return frame_number * PAGE_SIZE;
}

opt_uint64_t read_mmap(uint64_t request)
{
  uint64_t cur_num = 0;

  for (uint32_t i = 0; i < memory_regions_count; i++) {
    const memory_region_t* entry = &memory_regions[i];

    if (entry->type != MEMORY_AVAILABLE) {
      continue;
    }

    uint64_t entry_end = entry->addr + entry->len;

    for (uint64_t addr = entry->addr; addr + PAGE_SIZE <= entry_end;
         addr += PAGE_SIZE) {
      if (cur_num == request) {
        return (opt_uint64_t){ .has_value = true, .value = addr };
      }

      cur_num++;
    }
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

opt_uint64_t frame_number_to_request(frame_number_t frame_number)
{
  uintptr_t frame = frame_start_address(frame_number);

  uint64_t request = 0;
  for (uint32_t i = 0; i < memory_regions_count; i++) {
    const memory_region_t* entry = &memory_regions[i];
    if (entry->type != MEMORY_AVAILABLE) {
      continue;
    }

    uint64_t entry_end = entry->addr + entry->len;

    for (uint64_t addr = entry->addr; addr + PAGE_SIZE <= entry_end;
         addr += PAGE_SIZE) {
      if (addr == frame) {
        return (opt_uint64_t){ .has_value = true, .value = request };
      }

      request++;
    }
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

uint64_t frame_get_used_count()
{
  uint64_t count = 0;

  for (uint64_t i = 0; i < max_allocatable_frames; i++) {
    if (bitmap_get(allocated_frames, i)) {
      count++;
    }
  }

  return count;
}

uint64_t frame_get_max_count()
{
  return max_allocatable_frames;
}
