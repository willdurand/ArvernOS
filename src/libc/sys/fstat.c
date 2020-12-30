#include <sys/syscall.h>

int fstat(int fd, struct stat* statbuf)
{
#ifdef __is_libk
  return k_fstat(fd, statbuf);
#else
  errno = 0;

  int retval = syscall(SYSCALL_FSTAT, fd, statbuf);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
