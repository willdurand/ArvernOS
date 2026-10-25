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
  const memory_area_t areas[] = {
    {
      .start = 0x1100,
      .end = 0x2050,
      .flags = MEMORY_AREA_NO_ALLOC | MEMORY_AREA_IDENTITY,
    },
  };

  memory_reserved_areas_t reserved_areas = {
    .kernel_start = 0x1000,
    .kernel_end = 0x1100,
    .areas = areas,
    .area_count = 1,
  };

  const memory_region_t regions[NB_ENTRIES] = {
    { .type = MEMORY_AVAILABLE, .addr = 0x000000, .len = 0x10000 },
    { .type = MEMORY_RESERVED, .addr = 0x010000, .len = 0x05000 },
    { .type = MEMORY_AVAILABLE, .addr = 0x015000, .len = 0x03000 },
  };

  bitmap_t* bitmap = (bitmap_t*)calloc(1, PAGE_SIZE);

  describe("frame_init()");
  _frame_set_bitmap(bitmap);
  _frame_init(&reserved_areas, regions, NB_ENTRIES);
  end_describe();

  describe("frame_deallocate()");
  for (int i = 0; i < 6; i++) {
    opt_uint64_t frame = frame_allocate();
    frame_deallocate(frame_containing_address(frame.value));
  }
  end_describe();

  // This is the start address of the allocatable frames.
  uint64_t start_address = 0x4000;

  describe("frame_allocate()");
  for (int i = 0; i < 11; i++) {
    opt_uint64_t frame = frame_allocate();

    assert(frame.value == (start_address + 0x1000 * i), "allocates a frame");
    uint64_t frame_num = frame_containing_address(frame.value);
    assert(frame_num == frame_containing_address(start_address) + i,
           "returns the frame number");
    assert(frame_start_address(frame_num) == frame.value,
           "returns the correct address");
  }

  opt_uint64_t frame = frame_allocate();
  assert(frame.value == 0xf000,
         "allocates the last frame in the first available area");

  for (int i = 0; i < 3; i++) {
    opt_uint64_t frame = frame_allocate();
    assert(frame.value == 0x15000 + (i * 0x1000),
           "allocates a frame after reserved area (2nd area)");
  }
  end_describe();

  describe("frame_allocate() - out of memory");
  frame = frame_allocate();
  assert(frame.has_value == false, "should return no value");
  end_describe();

  describe("frame_deallocate()");
  uint64_t expected_frame = 0x5000;
  frame_deallocate(frame_containing_address(expected_frame));
  opt_uint64_t reused_frame = frame_allocate();
  assert(reused_frame.value == expected_frame,
         "allocates previously deallocated frame");
  end_describe();

  free(bitmap);

  return test_summary();
}
