#include <arch/time.h>

#include <board.h>
#include <drivers/bcm2835_timer.h>

void arch_timer_callback();

void arch_timer_init()
{
  bcm2835_timer_init(MMIO_BASE, &arch_timer_callback);
}

void arch_timer_callback()
{
  // TODO: Maybe implement this fucntion for AArch32/raspi2.
}

uint64_t arch_timer_uptime_microseconds()
{
  return bcm2835_timer_uptime_microseconds();
}
