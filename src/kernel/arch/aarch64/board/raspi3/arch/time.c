#include <arch/time.h>

#include <board.h>
#include <drivers/bcm2835_timer.h>
#include <time/fake_clock.h>

void arch_timer_callback();

void arch_timer_init()
{
  bcm2835_timer_init(MMIO_BASE, &arch_timer_callback);
}

void arch_timer_callback()
{
  // TODO: Maybe implement this fucntion for AArch64/raspi3.
}

uint64_t arch_timer_uptime_microseconds()
{
  return bcm2835_timer_uptime_microseconds();
}

void arch_clock_init()
{
  fake_clock_init();
}

uint64_t arch_clock_get_current_microseconds()
{
  return fake_clock_get_current_microseconds();
}
