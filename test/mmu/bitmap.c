#include <mmu/bitmap.h>
#include <stdlib.h>
#include <test.h>

int main()
{
  bitmap_t b[1];

  describe("bitmap get()/set()/clear()");
  assert(!bitmap_get(b, 0), "returns false when not set");
  bitmap_set(b, 0);
  assert(bitmap_get(b, 0), "returns true after set");
  bitmap_clear(b, 0);
  assert(!bitmap_get(b, 0), "returns false after clear");
  bitmap_set(b, BITS_PER_WORD - 1);
  assert(bitmap_get(b, BITS_PER_WORD - 1), "sets/gets the last possible bit");
  end_describe();

  return test_summary();
}
