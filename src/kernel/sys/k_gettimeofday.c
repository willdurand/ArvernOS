#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <sys/logging.h>
#include <time/clock.h>

int k_gettimeofday(struct timeval* p, void* z)
{
  UNUSED(*z);

  intmax_t current = clock_get_current_microseconds();

  // This represents the number of whole seconds of elapsed time.
  p->tv_sec = current / 1000000;
  // This is the rest of the elapsed time (a fraction of a second), represented
  // as the number of microseconds. It is always less than one million.
  p->tv_usec = current % 1000000;

  SYS_DEBUG("gettimeofday=%u", p->tv_sec);

  return 0;
}
