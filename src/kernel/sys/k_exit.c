#include <sys/k_syscall.h>

#include <proc/task.h>
#include <sys/logging.h>

void k_exit(int code)
{
  SYS_DEBUG("task id=%d is exiting (code=%d)", CURRENT_TASK->id, code);
  task_exit(code);
}
