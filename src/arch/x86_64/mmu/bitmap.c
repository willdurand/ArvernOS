#include "bitmap.h"

bool bitmap_get(const bitmap_t* b, uint64_t n)
{
  return b[WORD_OFFSET(n)] & ((bitmap_t)1 << (BIT_OFFSET(n))) ? true : false;
}

void bitmap_set(bitmap_t* b, uint64_t n)
{
  b[WORD_OFFSET(n)] |= (bitmap_t)1 << (BIT_OFFSET(n));
}

void bitmap_clear(bitmap_t* b, uint64_t n)
{
  b[WORD_OFFSET(n)] &= ~((bitmap_t)1 << (BIT_OFFSET(n)));
}
