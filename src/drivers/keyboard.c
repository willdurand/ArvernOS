#include "keyboard.h"
#include <core/isr.h>
#include <core/ports.h>
#include <stdlib.h>
#include <core/debug.h>

uint8_t last_scancode = 0;
uint8_t last_read = 0;

static void keyboard_callback(stack_t* stack) {
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    DEBUG("Received scancode: %d", scancode);
    last_scancode = scancode;
    last_read = 0;

    UNUSED(*stack);
}

void keyboard_init() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}

uint8_t keyboard_get_last_scancode() {
    if (last_read == 1) {
        return 0;
    }

    DEBUG("keyboard_get_last_scancode: %d", last_scancode);

    last_read = 1;

    return last_scancode;
}
