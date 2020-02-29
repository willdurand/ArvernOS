#include <sys/syscall.h>

ssize_t write(int fd, const void* buf, size_t count)
{
  ssize_t ret;

  __asm__(INT_SYSCALL
          : "=d"(ret)
          : "a"(SYSCALL_WRITE), "b"(fd), "c"(buf), "S"(count));

  return ret;
}
