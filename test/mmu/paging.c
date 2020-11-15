#include <mmu/paging.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <test.h>

#define NB_FRAMES 10

page_table_t* fake_p4;
page_table_t* fake_p3;
page_table_t* fake_p2;
page_table_t* fake_p1;

uint8_t next_table_index = 3;

char fake_frame[NB_FRAMES][PAGE_SIZE];
uint32_t next_free_frame = 0;

void kernel_panic(const char* format, ...)
{
  // noop implementation
}

opt_uint64_t test_frame_allocate()
{
  printf("new frame requested (next_free_frame=%d)\n", next_free_frame);

  if (next_free_frame < NB_FRAMES) {
    return (opt_uint64_t){ .has_value = true,
                           .value = (uint64_t)&fake_frame[next_free_frame++] };
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

void test_frame_deallocate(uint64_t frame_number)
{
  assert(frame_number == frame_containing_address((uint64_t)&fake_frame[0]),
         "deallocates the expected frame");
}

page_table_t* test_next_table()
{
  switch (next_table_index--) {
    case 3:
      return fake_p3;
    case 2:
      return fake_p2;
    case 1:
      return fake_p1;
    default:
      return 0;
  }
}

int main()
{
  fake_p4 = malloc(sizeof(page_table_t));
  _set_p4(fake_p4);

  fake_p3 = malloc(sizeof(page_table_t));
  fake_p2 = malloc(sizeof(page_table_t));
  fake_p1 = malloc(sizeof(page_table_t));

  printf("fake_p4=%p fake_p3=%p fake_p2=%p fake_p1=%p\n",
         fake_p4,
         fake_p3,
         fake_p2,
         fake_p1);

  uint64_t addr = 42 * 512 * 512 * 4096L; // 42th P3 entry
  uint64_t page = page_containing_address(addr);

  describe("page_containing_address()");
  assert(page == 11010048, "returns a page number for a given virtual address");
  assert(page_containing_address(0x1000) == 1,
         "returns a page number for a given virtual address");
  assert(page_containing_address(0x0000800000000000) == 0,
         "returns 0 for invalid addresses");
  assert(page_containing_address(0xffff800000000000 - 1) == 0,
         "returns 0 for invalid addresses");
  end_describe();

  describe("page_start_address()");
  assert(page_start_address(1) == 0x1000, "returns a virtual address");
  end_describe();

  describe("p1_index()");
  assert(_p1_index(page) == 0, "returns the correct p1 index");
  end_describe();

  describe("p2_index()");
  assert(_p2_index(page) == 0, "returns the correct p2 index");
  end_describe();

  describe("p3_index()");
  assert(_p3_index(page) == 42, "returns the correct p3 index");
  end_describe();

  describe("translate_page() and map()");
  assert(!translate_page(page).has_value, "returns no frame when not mapped");

  // reset fake next_table impl.
  next_table_index = 3;
  map(page, /* flags */ 0);

  // reset fake next_table impl.
  next_table_index = 3;
  assert(translate_page(page).has_value, "returns a frame when mapped");

  // reset fake next_table impl.
  next_table_index = 3;
  assert(translate_page(page).value ==
           frame_containing_address((uint64_t)&fake_frame[0]),
         "returns the expected frame");
  end_describe();

  describe("unmap()");
  // reset fake next_table impl.
  next_table_index = 3;
  unmap(page);

  // reset fake next_table impl.
  next_table_index = 3;
  assert(!translate_page(page).has_value, "returns no frame when not mapped");
  end_describe();

  free(fake_p1);
  free(fake_p2);
  free(fake_p3);
  free(fake_p4);

  return test_summary();
}
