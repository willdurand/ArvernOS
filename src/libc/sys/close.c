#include <sys/syscall.h>

int close(int fd)
{
#ifdef __is_libk
  return k_close(fd);
#else
  errno = 0;

  int retval = syscall(SYSCALL_CLOSE, fd);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
