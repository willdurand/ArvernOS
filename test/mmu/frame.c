#include <mmu/bitmap.h>
#include <mmu/frame.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

#define NB_ENTRIES 3

void kernel_panic(const char* format, ...)
{
  // noop implementation
}

int main()
{
  reserved_areas_t reserved_areas = { .kernel_start = 0x10000,
                                      .kernel_end = 0x11000,
                                      .multiboot_start = 0x12000,
                                      .multiboot_end = 0x13500 };

  multiboot_tag_mmap_t* mmap = malloc(
    sizeof(multiboot_tag_mmap_t) + sizeof(multiboot_mmap_entry_t) * NB_ENTRIES);

  multiboot_mmap_entry_t entries[NB_ENTRIES] = {
    { .type = MULTIBOOT_MEMORY_AVAILABLE, .addr = 0x000000, .len = 0x6000 },
    { .type = MULTIBOOT_MEMORY_RESERVED, .addr = 0x007000, .len = 0x5000 },
    { .type = MULTIBOOT_MEMORY_AVAILABLE, .addr = 0x012000, .len = 0x8000 },
  };
  mmap->size = sizeof(multiboot_mmap_entry_t) * NB_ENTRIES;
  mmap->entry_size = sizeof(multiboot_mmap_entry_t);
  memcpy(mmap->entries, entries, sizeof(multiboot_mmap_entry_t) * NB_ENTRIES);

  bitmap_t* bitmap = (bitmap_t*)malloc(PAGE_SIZE);

  describe("frame_init()");
  _frame_init(&reserved_areas, mmap);
  _frame_init_bitmap(bitmap);
  end_describe();

  describe("frame_deallocate()");
  for (int i = 0; i < 6; i++) {
    opt_uint64_t frame = frame_allocate();
    frame_deallocate(frame_containing_address(frame.value));
  }
  end_describe();

  describe("frame_allocate()");
  for (int i = 0; i < 6; i++) {
    opt_uint64_t frame = frame_allocate();

    assert(frame.value == (0x1000 * i), "allocates a frame");
    uint64_t frame_num = frame_containing_address(frame.value);
    assert(frame_num == i, "returns the frame number");
    assert(frame_start_address(frame_num) == frame.value,
           "returns the correct address");
  }
  opt_uint64_t frame = frame_allocate();
  assert(frame.value == 0x014000, "allocates a frame after reserved area");
  end_describe();

  describe("frame_deallocate()");
  uint64_t expected_frame = 0x2000;
  frame_deallocate(frame_containing_address(expected_frame));
  opt_uint64_t reused_frame = frame_allocate();
  assert(reused_frame.value == expected_frame,
         "allocates previously deallocated frame");
  end_describe();

  free(mmap);
  free(bitmap);

  return test_summary();
}
