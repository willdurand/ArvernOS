#include <sys/syscall.h>

int close(int fd)
{
#ifdef __is_libk
  return k_close(fd);
#else
  errno = 0;
  int retval;

  __asm__(INT_SYSCALL : "=d"(retval) : "a"(SYSCALL_CLOSE), "b"(fd));

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
