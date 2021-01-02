/** @file */
#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __is_libk

#define _HAVE_SIZE_T
#define _HAVE_UINTPTR_T

#include <liballoc.h>

#else

char* getenv(const char* name);

int putenv(char* str);

#endif

int atoi(char* s);

// Returns the absolute value of `x`.
int abs(int x);

#endif
