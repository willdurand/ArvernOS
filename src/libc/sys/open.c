#include <sys/syscall.h>

int open(const char* pathname, uint32_t flags)
{
#ifdef __is_libk
  return k_open(pathname, flags);
#else
  int fd = 0;

  __asm__(INT_SYSCALL
          : "=d"(fd)
          : "a"(SYSCALL_OPEN), "b"(pathname), "c"(flags));

  return fd;
#endif
}
