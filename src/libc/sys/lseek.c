#include <sys/syscall.h>

off_t lseek(int fd, off_t offset, int whence)
{
#ifdef __is_libk
  return k_lseek(fd, offset, whence);
#else
  off_t retval = syscall(SYSCALL_LSEEK, fd, offset, whence);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
