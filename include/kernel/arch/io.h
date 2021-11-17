#ifndef ARCH_IO_H
#define ARCH_IO_H

#include <stdbool.h>

unsigned char arch_getchar(bool blocking);

void arch_putchar(char c);

#endif
