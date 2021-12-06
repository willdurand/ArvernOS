#include <time/fake_clock.h>

#include <time/timer.h>

static uint64_t build_time = 0;

void fake_clock_init()
{
  build_time = BUILD_TIME_IN_NANOSECONDS / 1000;
}

uint64_t fake_clock_get_current_microseconds()
{
  return build_time + timer_uptime_microseconds();
}
