#include <sys/k_syscall.h>

#include <proc/logging.h>
#include <proc/task.h>

void k_exit(int code)
{
  PROC_DEBUG("task id=%d is exiting (code=%d)", CURRENT_TASK->id, code);
  task_exit(code);
}
