/** @file */
#ifndef KSHELL_H
#define KSHELL_H

#include <stdlib.h>

/// The number of characters to keep in the command line buffer.
#define READLINE_SIZE 256
/// The shell prompt.
#define PROMPT "kshell> "
/// The scancode for the <kbd>arrow up</kbd> key.
#define KB_ARROW_UP 72
/// The scancode for the <kbd>backspace</kbd> key.
#define KB_BACKSPACE 14
/// The scancode for the <kbd>caps lock</kbd> key.
#define KB_CAPS_LOCK 58
/// The scancode for the <kbd>enter</kbd> key.
#define KB_ENTER 28
/// The scancode for the left <kbd>ctrl</kbd> key.
#define KB_LCTRL 29
/// The scancode for the left <kbd>shift</kbd> key.
#define KB_LSHIFT 42
/// The scancode for the right <kbd>shift</kbd> key.
#define KB_RSHIFT 54
/// The scancode for the <kbd>tab</kbd> key.
#define KB_TAB 15

/**
 * Runs the kernel shell. This function takes a `scancode` as input and handles
 * the logic to convert it to a character and then run commands depending on
 * the command line buffer.
 *
 * @param scancode a scancode from the keyboard driver
 */
void kshell_run(uint8_t scancode);

/**
 * Prints the kernel shell prompt to the screen.
 */
void kshell_print_prompt();

#endif
