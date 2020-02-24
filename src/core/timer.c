#include "timer.h"
#include <core/isr.h>
#include <core/port.h>
#include <stdlib.h>

uint64_t tick = 0;

static void timer_callback(stack_t* stack)
{
  tick++;

  UNUSED(*stack);
}

uint64_t timer_tick()
{
  return tick;
}

uint64_t timer_uptime()
{
  return timer_tick() / TIMER_HZ;
}

void timer_init()
{
  isr_register_handler(IRQ0, timer_callback);

  uint64_t divisor = 1193180 / TIMER_HZ;

  port_byte_out(0x43, 0x36);
  port_byte_out(0x40, divisor & 0xff);
  port_byte_out(0x40, divisor >> 8);
}
