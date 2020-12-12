/** @file */
#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __is_libk

#define _HAVE_SIZE_T
#define _HAVE_UINTPTR_T

#include <liballoc/liballoc.h>

#endif

int atoi(char* s);

#endif
