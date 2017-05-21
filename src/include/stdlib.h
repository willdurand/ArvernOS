#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

#define UNUSED(x) (void)(x)

void itoa(int n, char* str, int base);
void ulltoa(uint64_t n, char* str, int base);

#endif
