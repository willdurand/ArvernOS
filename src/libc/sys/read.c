#include <sys/syscall.h>

ssize_t read(int fd, void* buf, size_t count)
{
#ifdef __is_libk
  return k_read(fd, buf, count);
#else
  ssize_t ret;

  __asm__(INT_SYSCALL
          : "=d"(ret)
          : "a"(SYSCALL_READ), "b"(fd), "c"(buf), "S"(count));

  return ret;
#endif
}
