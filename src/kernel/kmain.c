#include <core/isr.h>
#include <core/timer.h>
#include <core/debug.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <stdio.h>
#include "kmain.h"

void kmain(void)
{
    screen_init();
    screen_clear();

    printf("%s\n", KERNEL_ASCII);
    printf("%s %s / Built on: %s at %s\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE, KERNEL_TIME);

    isr_init();
    irq_init();

    // enable scheduler
    timer_init(50);

    // enable serial port
    serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
    DEBUG("%s has started\n", KERNEL_NAME);

    keyboard_init();

    while (1) {
        __asm__("hlt");
    }
}
