/** @file */
#ifndef MMU_BITMAP_H
#define MMU_BITMAP_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

/// This type represents a bitmap (bitset).
typedef uint64_t bitmap_t;

enum
{
  BITS_PER_WORD = sizeof(bitmap_t) * CHAR_BIT
};

#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

/**
 * Returns `true` when the bit is set, `false` otherwise.
 *
 * @param b a bitmap
 * @param n the bit to check
 * @return `true` when the bit to check is set, `false` otherwise
 */
bool bitmap_get(const bitmap_t* b, uint64_t n);

/**
 * Sets a bit (sets it to `true`).
 *
 * @param b a bitmap
 * @param n the bit to set
 */
void bitmap_set(bitmap_t* b, uint64_t n);

/**
 * Clears a bit (sets it to `false`).
 *
 * @param b a bitmap
 * @param n the bit to clear
 */
void bitmap_clear(bitmap_t* b, uint64_t n);

#endif
