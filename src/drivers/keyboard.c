#include "keyboard.h"
#include <core/isr.h>
#include <core/ports.h>
#include <stdio.h>
#include <stdlib.h>

#define SCANCODE_MAX 57

static void keyboard_callback(uint64_t stack)
{
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode > SCANCODE_MAX) {
        return;
    }

    printf("Received: %d\n", scancode);

    UNUSED(stack);
}

void keyboard_init()
{
    register_interrupt_handler(IRQ1, keyboard_callback);
}
