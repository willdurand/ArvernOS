#include <time/clock.h>

#include <arch/time.h>
#include <stdbool.h>

static bool initialized = false;

void clock_init()
{
  arch_clock_init();

  initialized = true;
}

uint64_t clock_get_current_microseconds()
{
  if (!initialized) {
    return 0;
  }

  return arch_clock_get_current_microseconds();
}
