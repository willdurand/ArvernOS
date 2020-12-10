#include <sys/syscall.h>

int socket(int domain, int type, int protocol)
{
  int fd = 0;

  __asm__(INT_SYSCALL
          : "=d"(fd)
          : "a"(SYSCALL_SOCKET), "b"(domain), "c"(type), "S"(protocol));

  return fd;
}
