#include "timer.h"
#include <core/isr.h>
#include <core/ports.h>
#include <drivers/screen.h>
#include <stdlib.h>

uint32_t tick = 0;

static void timer_callback(uint64_t stack)
{
    tick++;

    UNUSED(stack);
}

void init_timer(int freq)
{
    register_interrupt_handler(IRQ0, timer_callback);

    int divisor = 1193180 / freq;

    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xff);
    port_byte_out(0x40, divisor >> 8);
}
