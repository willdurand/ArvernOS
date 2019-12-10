#ifndef MMU_BITMAP_H
#define MMU_BITMAP_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint64_t bitmap_t;

enum { BITS_PER_WORD = sizeof(bitmap_t) * CHAR_BIT };

#define WORD_OFFSET(b)  ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)   ((b) % BITS_PER_WORD)

bool bitmap_get(bitmap_t* b, uint64_t n);

void bitmap_set(bitmap_t* b, uint64_t n);

void bitmap_clear(bitmap_t* b, uint64_t n);

#endif
