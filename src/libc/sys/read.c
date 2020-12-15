#include <sys/syscall.h>

ssize_t read(int fd, void* buf, size_t count)
{
#ifdef __is_libk
  return k_read(fd, buf, count);
#else
  errno = 0;
  ssize_t retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_READ), "b"(fd), "c"(buf), "S"(count));

  SYSCALL_SET_ERRNO(retval);

  return retval;
#endif
}
