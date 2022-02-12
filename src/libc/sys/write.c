#include <sys/syscall.h>

ssize_t write(int fd, const void* buf, size_t count)
{
#ifdef __is_libk
  return k_write(fd, buf, count);
#else
  ssize_t retval = syscall(SYSCALL_WRITE, fd, buf, count);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
