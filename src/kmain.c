#include <core/isr.h>
#include <drivers/screen.h>
#include <drivers/keyboard.h>
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

    keyboard_init();

    while (1) {
        __asm__("hlt");
    }
}
