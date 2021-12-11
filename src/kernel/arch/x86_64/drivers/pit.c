#include "pit.h"
#include <arvern/utils.h>
#include <core/interrupt.h>
#include <core/isr.h>
#include <core/logging.h>
#include <core/port.h>
#include <proc/task.h>
#include <stdlib.h>

static uint64_t tick = 0;

static void pit_callback(isr_stack_t* stack)
{
  UNUSED(*stack);

  tick++;

  task_schedule_on_timer_tick();
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
  // The data rate is actually a 'divisor' register for this device. The timer
  // will divide it's input clock of 1.19MHz (1193180Hz) by the number you give
  // it in the data register to figure out how many times per second to fire
  // the signal for that channel.
  //
  // Source: http://www.osdever.net/bkerndev/Docs/pit.htm
  uint64_t divisor = 1193180 / PIT_FREQUENCY_HZ;

  INFO("pit: initialize with divisor=%d", divisor);

  // 0x34 = 00110100
  //
  // 00       : Channel 0
  //   11     : Access mode: lobyte/hibyte
  //     010  : Mode 2 (rate generator)
  //        0 : 16-bit binary
  port_byte_out(0x43, 0x34);
  port_byte_out(0x40, divisor & 0xff);
  port_byte_out(0x40, divisor >> 8);

  isr_register_handler(IRQ0, pit_callback);
}
