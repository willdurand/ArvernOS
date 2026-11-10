#include <sys/k_syscall.h>

#include <proc/task.h>
#include <sys/logging.h>

pid_t k_waitpid(pid_t pid, int* status)
{
  pid_t r = task_wait(pid, status);

  SYS_DEBUG("waitpid: pid=%d -> %d status=%d",
            (int)pid,
            (int)r,
            status != NULL ? *status : -999);

  return r;
}
