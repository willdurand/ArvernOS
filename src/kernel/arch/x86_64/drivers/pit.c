#include "pit.h"
#include <arvern/utils.h>
#include <core/interrupt.h>
#include <core/isr.h>
#include <core/port.h>
#include <stdlib.h>

static uint64_t tick = 0;

static void pit_callback(isr_stack_t* stack)
{
  tick++;

  UNUSED(*stack);
}

uint64_t pit_tick()
{
  return tick;
}

uint64_t pit_uptime()
{
  return pit_tick() / PIT_FREQUENCY_HZ;
}

void pit_init()
{
  isr_register_handler(IRQ0, pit_callback);
  // The data rate is actually a 'divisor' register for this device. The timer
  // will divide it's input clock of 1.19MHz (1193180Hz) by the number you give
  // it in the data register to figure out how many times per second to fire
  // the signal for that channel.
  //
  // Source: http://www.osdever.net/bkerndev/Docs/pit.htm
  uint64_t divisor = 1193180 / PIT_FREQUENCY_HZ;

  port_byte_out(0x43, 0x36);
  port_byte_out(0x40, divisor & 0xff);
  port_byte_out(0x40, divisor >> 8);
}
