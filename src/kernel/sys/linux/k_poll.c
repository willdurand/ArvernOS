#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <sys/logging.h>
#include <sys/poll.h>
#include <time/timer.h>

int k_poll(struct pollfd* fds, nfds_t nfds, int timeout)
{
  UNUSED(*fds);
  UNUSED(nfds);
  MAYBE_UNUSED(timeout);

  SYS_DEBUG("timeout=%d", timeout);

  uint64_t now = timer_uptime_microseconds();

  while (timer_uptime_microseconds() < (now + 1000)) {
    ;
  }

  return 0;
}
