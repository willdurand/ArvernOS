#include <sys/syscall.h>

ssize_t read(int fd, void* buf, size_t count)
{
#ifdef __is_libk
  return k_read(fd, buf, count);
#else
  errno = 0;

  ssize_t retval = syscall(SYSCALL_READ, fd, buf, count);
  ;

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
