#include <sys/k_syscall.h>

#include <proc/task.h>

pid_t k_getpid()
{
  return CURRENT_TASK->id;
}
