#include <sys/syscall.h>

int open(const char* pathname, uint32_t flags, ...)
{
  int fd;

  __asm__(INT_SYSCALL
          : "=d"(fd)
          : "a"(SYSCALL_OPEN), "b"(pathname), "c"(flags));

  return fd;
}
