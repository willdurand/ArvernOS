#include <sys/syscall.h>

int fstat(int fd, struct stat* statbuf)
{
  int ret;

  __asm__(INT_SYSCALL : "=d"(ret) : "a"(SYSCALL_FSTAT), "b"(fd), "c"(statbuf));

  return ret;
}
