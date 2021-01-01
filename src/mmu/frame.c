#include "frame.h"
#include "logging.h"
#include <kernel/panic.h>
#include <string.h>

opt_uint64_t read_mmap(uint64_t request);

static multiboot_tag_mmap_t* memory_area = NULL;
static uint64_t kernel_start;
static uint64_t kernel_end;
static uint64_t multiboot_start;
static uint64_t multiboot_end;
static uint64_t max_frames;

// These variables can be accessed by other parts of the kernel and we do that
// in `paging_init()` to identity map the bitmap for allocated frames.
uint8_t frames_for_bitmap = 0;
bitmap_t* allocated_frames = NULL;

void frame_init(multiboot_info_t* mbi)
{
  reserved_areas_t reserved = find_reserved_areas(mbi);
  multiboot_tag_mmap_t* mmap =
    (multiboot_tag_mmap_t*)find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);

  _frame_init(&reserved, mmap);

  opt_uint64_t frame = read_mmap(0);
  if (!frame.has_value) {
    PANIC("could not get a frame to initialize the frame allocator");
  }

  // `0x500` should put us in a good enough place, see:
  // https://wiki.osdev.org/Memory_Map_(x86)#Overview
  _frame_init_bitmap((bitmap_t*)((uint64_t)frame.value + 0x500));

  INFO("initialized frame allocator with multiboot_start = %p "
       "multiboot_end=%p kernel_start=%p kernel_end=%p max_frames=%lld "
       "allocated_frames=%p used_count=%lld",
       multiboot_start,
       multiboot_end,
       kernel_start,
       kernel_end,
       max_frames,
       allocated_frames,
       frame_get_used_count());
}

void _frame_init(reserved_areas_t* reserved, multiboot_tag_mmap_t* mmap)
{
  memory_area = mmap;
  kernel_start = reserved->kernel_start;
  kernel_end = reserved->kernel_end;
  multiboot_start = reserved->multiboot_start;
  multiboot_end = reserved->multiboot_end;

  MMU_DEBUG("multiboot_start = %p, multiboot_end = %p",
            reserved->multiboot_start,
            reserved->multiboot_end);
  MMU_DEBUG("kernel_start    = %p, kernel_end    = %p",
            reserved->kernel_start,
            reserved->kernel_end);

  uint64_t available_memory = 0;
  for (multiboot_mmap_entry_t* entry = memory_area->entries;
       (uint8_t*)entry < (uint8_t*)memory_area + memory_area->size;
       entry =
         (multiboot_mmap_entry_t*)((uint64_t)entry + memory_area->entry_size)) {
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
    }

    available_memory += entry->len;
  }

  max_frames = available_memory / PAGE_SIZE;
  frames_for_bitmap =
    ((max_frames / BITS_PER_WORD) * sizeof(bitmap_t)) / PAGE_SIZE + 1;
}

void _frame_init_bitmap(bitmap_t* addr)
{
  allocated_frames = addr;
  memset(allocated_frames, 0, (frames_for_bitmap + 1) * PAGE_SIZE);

  for (uint64_t i = 0; i <= frames_for_bitmap; i++) {
    bitmap_set(allocated_frames,
               frame_containing_address((uint64_t)allocated_frames) + i);
  }
}

opt_uint64_t frame_allocate()
{
  opt_uint64_t frame_number = (opt_uint64_t){ .has_value = false, .value = 0 };

  for (uint64_t i = 0; i < max_frames; i++) {
    if (bitmap_get(allocated_frames, i) == false) {
      frame_number.value = i;
      frame_number.has_value = true;
      break;
    }
  }

  if (frame_number.has_value) {
    opt_uint64_t frame = read_mmap(frame_number.value);

    if (frame.has_value) {
      MMU_DEBUG(
        "allocated frame=%lld addr=%p", frame_number.value, frame.value);
      bitmap_set(allocated_frames, frame_number.value);
    }

    return frame;
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

void frame_deallocate(frame_number_t frame_number)
{
  MMU_DEBUG("deallocating frame=%lld", frame_number);
  bitmap_clear(allocated_frames, frame_number);
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

  for (multiboot_mmap_entry_t* entry = memory_area->entries;
       (uint8_t*)entry < (uint8_t*)memory_area + memory_area->size;
       entry =
         (multiboot_mmap_entry_t*)((uint64_t)entry + memory_area->entry_size)) {
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
    }

    uint64_t entry_end = entry->addr + entry->len;

    for (uint64_t addr = entry->addr; addr + PAGE_SIZE <= entry_end;
         addr += PAGE_SIZE) {
      if (addr >= kernel_start && addr <= multiboot_end) {
        continue;
      }

      if (cur_num == request) {
        return (opt_uint64_t){ .has_value = true, .value = addr };
      }

      cur_num++;
    }
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

uint64_t frame_get_used_count()
{
  uint64_t count = 0;

  for (uint64_t i = 0; i < max_frames; i++) {
    if (bitmap_get(allocated_frames, i)) {
      count++;
    }
  }

  return count;
}

uint64_t frame_get_max_count()
{
  return max_frames;
}

void frame_mark_as_used(uint64_t physical_address)
{
  frame_number_t frame = frame_containing_address(physical_address);

  if (frame < max_frames) {
    MMU_DEBUG("marking frame=%lld (addr=%p) as used", frame, physical_address);
    bitmap_set(allocated_frames, frame);
  } else {
    MMU_DEBUG("not marking frame=%lld (addr=%p) as used because address does "
              "not point to usable RAM",
              frame,
              physical_address);
  }
}
