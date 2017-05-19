#include <stdint.h>
#include <drivers/screen.h>
#include "kmain.h"

void kmain(void)
{
    screen_init();
    screen_clear();

    screen_print(KERNEL_NAME);
    screen_print(" ");
    screen_print(KERNEL_VERSION);
    screen_print(" / Built on: ");
    screen_print(KERNEL_DATE);
    screen_print(" at ");
    screen_print(KERNEL_TIME);
}
