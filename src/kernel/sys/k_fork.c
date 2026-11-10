#include <sys/k_syscall.h>

#include <errno.h>
#include <proc/task.h>
#include <sys/logging.h>

int k_fork(void)
{
  task_t* task = CURRENT_TASK;

  if (task == NULL) {
    return -EINVAL;
  }

  // Kernel tasks (idle, kinit, ...) are not allowed to fork.
  if (task->flags & KERNEL_TASK) {
    return -EPERM;
  }

#if defined(__x86_64__)
  task_t* child = task_fork_child(task);
  if (child == NULL) {
    return -EAGAIN;
  }

  SYS_DEBUG("fork: parent=%d child=%d", task->id, child->id);

  // The child's syscall return value (0) is seeded in its kernel stack by
  // `arch_task_create_child()` / `ret_from_fork_user`, not here.
  return child->id;
#else
  // Address spaces / user entry are not wired up for ARM yet.
  return -ENOSYS;
#endif
}
