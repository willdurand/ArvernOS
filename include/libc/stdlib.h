/** @file */
#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __is_libk

#ifdef CONFIG_USE_DLMALLOC

#include <dlmalloc.h>

#else // CONFIG_USE_DLMALLOC

#define _HAVE_SIZE_T
#define _HAVE_UINTPTR_T

#include <liballoc.h>

#endif // CONFIG_USE_DLMALLOC

#else

char* getenv(const char* name);

int putenv(char* str);

#endif

int atoi(char* s);

// Returns the absolute value of `x`.
int abs(int x);

long int strtol_wip(const char* str, char** end, int base);

void abort();

#endif
