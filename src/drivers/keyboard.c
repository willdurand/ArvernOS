#include "keyboard.h"
#include <core/debug.h>
#include <core/isr.h>
#include <core/ports.h>
#include <stdlib.h>

uint8_t last_scancode = 0;

static void keyboard_callback(stack_t* stack) {
    uint8_t status = port_byte_in(KEYBOARD_STATUS_PORT);

    if (status & 0x1) {
        uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

        DEBUG("received scancode: %d", scancode);
        last_scancode = scancode;
    }

    UNUSED(*stack);
}

void keyboard_init() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}

uint8_t keyboard_get_last_scancode() {
    uint8_t scancode = last_scancode;
    last_scancode = 0;

    return scancode;
}
