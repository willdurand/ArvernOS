#include <mmu/bitmap.h>
#include <stdlib.h>
#include <test.h>

int main()
{
  bitmap_t b[2];

  describe("bitmap init");
  memset(b, 0, sizeof(bitmap_t) * 2);
  for (uint32_t i = 0; i < BITS_PER_WORD * 2; i++) {
    assert(!bitmap_get(b, i), "returns false by default");
  }
  end_describe();

  describe("bitmap get()/set()/clear()");
  bitmap_set(b, 0);
  assert(bitmap_get(b, 0), "returns true after set");
  bitmap_clear(b, 0);
  assert(!bitmap_get(b, 0), "returns false after clear");
  bitmap_set(b, BITS_PER_WORD - 1);
  assert(bitmap_get(b, BITS_PER_WORD - 1), "sets/gets the last possible bit");
  end_describe();

  return test_summary();
}
