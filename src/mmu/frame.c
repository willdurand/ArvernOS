#include "frame.h"
#include <kernel/panic.h>
#include <mmu/bitmap.h>
#include <mmu/debug.h>
#include <string.h>

uint64_t read_mmap(uint64_t request);

multiboot_tag_mmap_t* memory_area;
uint64_t kernel_start;
uint64_t kernel_end;
uint64_t multiboot_start;
uint64_t multiboot_end;
bitmap_t allocated_frames[MAX_FRAMES / BITS_PER_WORD];

void frame_init(multiboot_info_t* mbi)
{
  reserved_areas_t reserved = find_reserved_areas(mbi);
  multiboot_tag_mmap_t* mmap = find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);

  MMU_DEBUG("multiboot_start = %p, multiboot_end = %p",
            reserved.multiboot_start,
            reserved.multiboot_end);
  MMU_DEBUG("kernel_start    = %p, kernel_end    = %p",
            reserved.kernel_start,
            reserved.kernel_end);

  memory_area = mmap;
  kernel_start = reserved.kernel_start;
  kernel_end = reserved.kernel_end;
  multiboot_start = reserved.multiboot_start;
  multiboot_end = reserved.multiboot_end;
  memset(allocated_frames, 0, MAX_FRAMES / BITS_PER_WORD);

  DEBUG("Initialized frame allocator with multiboot_start = %p "
        "multiboot_end=%p kernel_start=%p kernel_end=%p",
        multiboot_start,
        multiboot_end,
        kernel_start,
        kernel_end);
}

uint64_t read_mmap(uint64_t request)
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

    for (uint64_t addr = entry->addr; addr + PAGE_SIZE < entry_end;
         addr += PAGE_SIZE) {
      if ((addr >= multiboot_start && addr <= multiboot_end) ||
          (addr >= kernel_start && addr <= kernel_end)) {
        continue;
      }

      if (cur_num == request && addr != 0) {
        return addr;
      }

      cur_num++;
    }
  }

  return 0;
}

uint64_t frame_allocate()
{
  uint64_t free_frame = 0;

  for (uint64_t i = 1; i <= MAX_FRAMES; i++) {
    if (bitmap_get(allocated_frames, i) == false) {
      free_frame = i;
      break;
    }
  }

  uint64_t addr = read_mmap(free_frame);

  if (addr == 0) {
    PANIC("%s", "failed to allocate a new frame");
  }

  MMU_DEBUG("allocated frame with addr=%p free_frame=%u", addr, free_frame);
  bitmap_set(allocated_frames, free_frame);

  return addr;
}

void frame_deallocate(uint64_t frame_number)
{
  uint64_t addr = frame_start_address(frame_number);

  MMU_DEBUG("deallocating frame=%u addr=%p", frame_number, addr);

  bitmap_clear(allocated_frames, frame_number);
  memset((void*)addr, 0, PAGE_SIZE);
}

uint64_t frame_containing_address(uint64_t physical_address)
{
  return physical_address / PAGE_SIZE;
}

uint64_t frame_start_address(uint64_t frame_number)
{
  return frame_number * PAGE_SIZE;
}
