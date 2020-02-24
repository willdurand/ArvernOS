/**
 * @file
 * @see https://wiki.osdev.org/PS/2_Keyboard
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

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
