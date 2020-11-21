#include <sys/syscall.h>

int close(int fd)
{
  int ret = 0;

  __asm__(INT_SYSCALL : "=d"(ret) : "a"(SYSCALL_CLOSE), "b"(fd));

  return ret;
}
