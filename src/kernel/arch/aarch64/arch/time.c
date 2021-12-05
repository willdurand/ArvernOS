#include <arch/time.h>

#include <arvern/utils.h>

void arch_gettimeofday(struct timeval* p, void* z)
{
  UNUSED(*z);

  // TODO:

  p->tv_sec = 0;
  p->tv_usec = 0;
}
