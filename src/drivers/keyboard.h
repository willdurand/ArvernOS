#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64

void keyboard_init();
uint8_t keyboard_get_last_scancode();

#endif
