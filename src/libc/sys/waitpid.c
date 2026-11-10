#include <sys/syscall.h>

pid_t waitpid(pid_t pid, int* status)
{
#ifdef __is_libk
  return k_waitpid(pid, status);
#else
  pid_t retval = syscall(SYSCALL_WAITPID, pid, status);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
