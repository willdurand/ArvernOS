#include <sys/syscall.h>

int close(int fd)
{
#ifdef __is_libk
  return k_close(fd);
#else
  int ret = 0;

  __asm__(INT_SYSCALL : "=d"(ret) : "a"(SYSCALL_CLOSE), "b"(fd));

  return ret;
#endif
}
