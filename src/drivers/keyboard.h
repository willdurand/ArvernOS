#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdlib.h>

#define KEYBOARD_DATA_PORT 0x60

void keyboard_init();
uint8_t keyboard_get_last_scancode();

#endif
