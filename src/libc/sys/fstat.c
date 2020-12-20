#include <sys/syscall.h>

int fstat(int fd, struct stat* statbuf)
{
#ifdef __is_libk
  return k_fstat(fd, statbuf);
#else
  errno = 0;
  int retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_FSTAT), "b"(fd), "c"(statbuf));

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
