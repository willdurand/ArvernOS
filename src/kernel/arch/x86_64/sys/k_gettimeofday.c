#include <sys/k_syscall.h>

#include <drivers/cmos.h>
#include <drivers/timer.h>
#include <sys/logging.h>
#include <willos/utils.h>

int k_gettimeofday(struct timeval* p, void* z)
{
  UNUSED(*z);

  p->tv_sec = cmos_boot_time() + timer_uptime();
  // TODO: set a correct value, see:
  // https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
  p->tv_usec = 0;

  SYS_DEBUG("gettimeofday=%u", p->tv_sec);

  return 0;
}
