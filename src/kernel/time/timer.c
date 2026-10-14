#include <time/timer.h>

#include <arch/time.h>
#include <initcall.h>
#include <logging.h>
#include <stdbool.h>

static bool initialized = false;

int timer_init()
{
  INFO("%s", "time: initialize timer");

  arch_timer_init();

  initialized = true;

  return 0;
}

uint64_t timer_uptime()
{
  return MICROSEC_TO_SEC(timer_uptime_microseconds());
}

uint64_t timer_uptime_microseconds()
{
  if (!initialized) {
    return 0;
  }

  return arch_timer_uptime_microseconds();
}

initcall_core_register_with_priority(timer_init, 2);
