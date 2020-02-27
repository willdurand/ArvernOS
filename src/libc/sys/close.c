#include <sys/syscall.h>

int close(int fd)
{
  int ret;

  __asm__(INT_SYSCALL : "=d"(ret) : "a"(SYSCALL_CLOSE), "b"(fd));

  return ret;
}
