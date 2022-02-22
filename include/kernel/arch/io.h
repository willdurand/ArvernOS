/**
 * @file
 *
 * Architecture-specific code for input/output. Currently, "input" and "output"
 * are not well-defined, although the functions listed below should be used
 * exclusively by the kernel. Input should be "the thing that sends characters
 * typed by a human to the kernel" and output should be "the thing that sends
 * characters from the kernel to a human".
 */
#ifndef ARCH_IO_H
#define ARCH_IO_H

#include <stdbool.h>

/**
 * Reads a character from an input.
 *
 * @param blocking whether the call to this function should wait for a character
 * or not
 * @return a character
 */
unsigned char arch_getchar(bool blocking);

/**
 * Prints a character to an output.
 *
 * @param c the character to print
 */
void arch_putchar(char c);

#endif
