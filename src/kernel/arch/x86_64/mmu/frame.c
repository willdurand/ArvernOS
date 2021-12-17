#include "frame.h"
#include <inttypes.h>
#include <mmu/logging.h>
#include <panic.h>
#include <string.h>

opt_uint64_t read_mmap(uint64_t request);
uint64_t frame_number_to_request(frame_number_t frame_number);

// These variables can be accessed by other parts of the kernel and we do that
// in `paging_init()` to identity map the bitmap for allocated frames.
uint8_t frames_for_bitmap = 0;
bitmap_t* allocated_frames = NULL;

static multiboot_tag_mmap_t* mmap = NULL;
static uint64_t max_allocatable_frames = 0;
static uint64_t first_request = 0;

void frame_init()
{
  INFO("%s", "mmu: initialize frame allocator");

  reserved_areas_t reserved_areas = multiboot_get_reserved_areas();
  multiboot_tag_mmap_t* _mmap = multiboot_get_mmap();

  _frame_init(&reserved_areas, _mmap);

  DEBUG("start=%p end=%p max_allocatable_frames=%" PRIu64
        " allocated_frames=%p used_count=%" PRIu64 " first_request=%" PRIu64,
        reserved_areas.start,
        reserved_areas.end,
        max_allocatable_frames,
        allocated_frames,
        frame_get_used_count(),
        first_request);
}

void _frame_init(reserved_areas_t* reserved_areas, multiboot_tag_mmap_t* _mmap)
{
  mmap = _mmap;

  uint64_t available_memory = 0;
  for (multiboot_mmap_entry_t* entry = mmap->entries;
       (uint8_t*)entry < (uint8_t*)mmap + mmap->size;
       entry = (multiboot_mmap_entry_t*)((uint64_t)entry + mmap->entry_size)) {
    available_memory += entry->len;
  }

  max_allocatable_frames = available_memory / PAGE_SIZE;
  frames_for_bitmap =
    ((max_allocatable_frames / BITS_PER_WORD) * sizeof(bitmap_t)) / PAGE_SIZE +
    1;

  uint64_t bitmap_address =
    frame_start_address(frame_containing_address(reserved_areas->end) + 1);
  uint64_t bitmap_size = frames_for_bitmap * PAGE_SIZE;
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
  for (uint64_t i = 0; i < end; i++) {
    bitmap_set(allocated_frames, i);
  }
  first_request = end;
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
  uint64_t request = frame_number_to_request(frame_number);
  MMU_DEBUG(
    "deallocating frame=%" PRIu64 " request=%" PRIu64, frame_number, request);

  if (request == 0) {
    DEBUG("failed to deallocate frame=%" PRIu64 " because request=%" PRIu64
          ", which it is "
          "very suspicious",
          frame_number,
          request);
    return;
  }

  bitmap_clear(allocated_frames, request);
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

  for (multiboot_mmap_entry_t* entry = mmap->entries;
       (uint8_t*)entry < (uint8_t*)mmap + mmap->size;
       entry = (multiboot_mmap_entry_t*)((uint64_t)entry + mmap->entry_size)) {
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
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

uint64_t frame_number_to_request(frame_number_t frame_number)
{
  uintptr_t frame = frame_start_address(frame_number);

  uint64_t request = 0;
  for (multiboot_mmap_entry_t* entry = mmap->entries;
       (uint8_t*)entry < (uint8_t*)mmap + mmap->size;
       entry = (multiboot_mmap_entry_t*)((uint64_t)entry + mmap->entry_size)) {
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
    }

    uint64_t entry_end = entry->addr + entry->len;

    for (uint64_t addr = entry->addr; addr + PAGE_SIZE <= entry_end;
         addr += PAGE_SIZE) {
      if (addr == frame) {
        return request;
      }

      request++;
    }
  }

  return 0;
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
