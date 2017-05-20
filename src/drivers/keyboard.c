#include "keyboard.h"
#include <core/isr.h>
#include <core/ports.h>
#include <drivers/screen.h>
#include <stdlib.h>
#include <string.h>

#define SCANCODE_MAX 57

static void keyboard_callback(uint64_t stack)
{
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode > SCANCODE_MAX) {
        return;
    }

    char s[3];
    itoa(scancode, s);

    screen_print("Received: ");
    screen_print(s);
    screen_print("\n");

    UNUSED(stack);
}

void keyboard_init()
{
    register_interrupt_handler(IRQ1, keyboard_callback);
}
