#include "logging.h"
#include "syscall.h"
#include <drivers/cmos.h>
#include <drivers/timer.h>
#include <sys/time.h>

void syscall_gettimeofday(registers_t* registers)
{
  struct timeval* t = (struct timeval*)registers->rbx;

  t->tv_sec = cmos_boot_time() + timer_uptime();
  // TODO: set a correct value, see:
  // https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
  t->tv_usec = 0;

  CORE_SYS_DEBUG("gettimeofday=%u", t->tv_sec);
}
