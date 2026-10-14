#include <time/clock.h>

#include <arch/time.h>
#include <initcall.h>
#include <logging.h>
#include <stdbool.h>

static bool initialized = false;

int clock_init()
{
  INFO("%s", "time: initialize clock");

  arch_clock_init();

  initialized = true;

  return 0;
}

uint64_t clock_get_current_microseconds()
{
  if (!initialized) {
    return 0;
  }

  return arch_clock_get_current_microseconds();
}

initcall_core_register_with_priority(clock_init, 3);
