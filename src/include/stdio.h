/** @file */
#ifndef STDIO_H
#define STDIO_H

#ifdef __is_libk
#include <printf/printf.h>

/**
 * This function is needed for the `printf/printf.h` lib.
 */
void _putchar(char c);
#else

void printf(const char* s);

/**
 * Writes a character to the screen.
 *
 * @param c a character to be written
 */
void putchar(char c);
#endif

#endif
