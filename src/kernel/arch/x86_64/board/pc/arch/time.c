#include <arch/time.h>

#include <drivers/cmos.h>
#include <drivers/pit.h>

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
  cmos_init();
}

uint64_t arch_clock_get_current_microseconds()
{
  return SEC_TO_MICROSEC(cmos_current_time());
}
