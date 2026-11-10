#include <sys/syscall.h>

pid_t fork(void)
{
#ifdef __is_libk
  return k_fork();
#else
  pid_t retval = syscall(SYSCALL_FORK);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
