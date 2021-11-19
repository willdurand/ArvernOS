#include <sys/k_syscall.h>

#include <arch/time.h>
#include <sys/logging.h>

int k_gettimeofday(struct timeval* p, void* z)
{
  arch_gettimeofday(p, z);
  SYS_DEBUG("gettimeofday=%u", p->tv_sec);

  return 0;
}
