/** @file */
#ifndef STDIO_H
#define STDIO_H

#include <printf/printf.h>

/**
 * This function is needed for the `printf/printf.h` lib.
 */
void _putchar(char c);

#ifdef __is_libc

/**
 * Writes a character to the screen.
 *
 * @param c a character to be written
 */
void putchar(char c);

/**
 * Gets a character (an unsigned char) from the keyboard.
 *
 * @return the character read
 */
char getchar();

#endif

#endif
