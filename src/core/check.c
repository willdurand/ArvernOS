#include "check.h"
#include <stdio.h>
#include <core/timer.h>

void check_interrupts()
{
    printf("- checking interrupts... ");

    uint32_t tick = timer_tick();
    while (tick == timer_tick()) ;

    printf("OK\n");
}
