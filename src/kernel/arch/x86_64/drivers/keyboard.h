/**
 * @file
 * @see https://wiki.osdev.org/PS/2_Keyboard
 */
#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

/// The scancode for the <kbd>arrow up</kbd> key.
#define KB_ARROW_UP 72
/// The scancode for the <kbd>backspace</kbd> key.
#define KB_BACKSPACE 14
/// The scancode for the <kbd>caps lock</kbd> key.
#define KB_CAPS_LOCK 58
/// The scancode for the <kbd>enter</kbd> key.
#define KB_ENTER 28
/// The scancode for the left <kbd>shift</kbd> key.
#define KB_LSHIFT 42
/// The scancode for the right <kbd>shift</kbd> key.
#define KB_RSHIFT 54

/**
 * Initializes the keyboard (PS/2 controller) driver.
 */
void keyboard_init();

/**
 * Returns the last scancode read from the keyboard or `0`.
 *
 * @see https://en.wikipedia.org/wiki/Scancode
 *
 * @return the last scancode read or `0`
 */
uint8_t keyboard_get_scancode();

#endif
