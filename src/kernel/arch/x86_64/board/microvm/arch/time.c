#include <arch/time.h>

#include <drivers/pit.h>
#include <time/fake_clock.h>

void arch_timer_init()
{
  pit_init();
}

uint64_t arch_timer_uptime_microseconds()
{
  return pit_tick() * 10000;
}

void arch_clock_init()
{
  fake_clock_init();
}

uint64_t arch_clock_get_current_microseconds()
{
  return fake_clock_get_current_microseconds();
}
