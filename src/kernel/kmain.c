#include <core/isr.h>
#include <core/timer.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <stdio.h>
#include "kmain.h"

void kmain(void)
{
    screen_init();
    screen_clear();

    screen_print(KERNEL_ASCII);
    screen_print("\n");

    screen_print(KERNEL_NAME);
    screen_print(" ");
    screen_print(KERNEL_VERSION);
    screen_print(" / Built on: ");
    screen_print(KERNEL_DATE);
    screen_print(" at ");
    screen_print(KERNEL_TIME);
    screen_print("\n");

    isr_init();
    irq_init();

    // enable scheduler
    timer_init(50);

    // enable serial port
    serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
    serial_print(SERIAL_COM1, KERNEL_NAME);
    serial_print(SERIAL_COM1, " has started");
    serial_print(SERIAL_COM1, "\n");

    keyboard_init();

    while (1) {
        __asm__("hlt");
    }
}
