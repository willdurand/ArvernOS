/** @file */
#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define _HAVE_SIZE_T
#define _HAVE_UINTPTR_T

#include <liballoc/liballoc.h>

int atoi(char* s);
int abs(int x);

__attribute__((__noreturn__)) void abort(void);

#endif
