/** @file */
#ifndef STDIO_H
#define STDIO_H

#include <printf.h>

/**
 * This function is needed for the `printf/printf.h` lib.
 */
void _putchar(char c);

#ifndef __is_libk

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
unsigned char getchar();

#endif

#endif
