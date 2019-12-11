/** @file */
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64

/**
 * Initializes the keyboard driver.
 */
void keyboard_init();

/**
 * Returns the last scancode read from the keyboard or `0`.
 *
 * @return the last scancode read or `0`
 */
uint8_t keyboard_get_last_scancode();

#endif
