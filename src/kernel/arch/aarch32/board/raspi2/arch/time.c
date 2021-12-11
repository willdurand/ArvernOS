#include <arch/time.h>

#include <board.h>
#include <drivers/bcm2835_timer.h>
#include <proc/task.h>

void arch_timer_callback();

void arch_timer_init()
{
  bcm2835_timer_init(MMIO_BASE, &arch_timer_callback);
}

void arch_timer_callback()
{
  task_schedule_on_timer_tick();
}

uint64_t arch_timer_uptime_microseconds()
{
  return bcm2835_timer_uptime_microseconds();
}
