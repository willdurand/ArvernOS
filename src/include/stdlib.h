#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define _HAVE_SIZE_T

#include <liballoc/liballoc.h>

#define UNUSED(x) (void)(x)

void itoa(int n, char* str, int base);
void ulltoa(uint64_t n, char* str, int base);

#endif
