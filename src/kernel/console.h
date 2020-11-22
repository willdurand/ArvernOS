/** @file */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

/**
 * Initializes the console (VT100).
 */
void console_init();

/**
 * Prints a character to the console.
 *
 * @param c the character to print
 */
void console_putchar(char c);

/**
 * Writes a string to the console.
 *
 * @param s the string to write to the console
 * @param count the number of characters to write
 * @return the number of characters printed to the console
 */
size_t console_write(const char* s, size_t count);

#endif
