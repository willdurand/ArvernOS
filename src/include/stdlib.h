/** @file */
#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define _HAVE_SIZE_T
#define _HAVE_UINTPTR_T

#include <liballoc/liballoc.h>

/// Marks a variable as unused so that the compiler does not raise a warning
/// and we are aware of this variable not being used on purpose.
#define UNUSED(x) (void)(x)

void itoa(int n, char* str, int base);

void ulltoa(uint64_t n, char* str, int base);

#endif
