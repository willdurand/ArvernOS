#include <sys/syscall.h>

ssize_t write(int fd, const void* buf, size_t count)
{
#ifdef __is_libk
  return k_write(fd, buf, count);
#else
  errno = 0;
  ssize_t retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_WRITE), "b"(fd), "c"(buf), "S"(count));

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
