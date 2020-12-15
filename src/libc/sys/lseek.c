#include <sys/syscall.h>

off_t lseek(int fd, off_t offset, int whence)
{
#ifdef __is_libk
  return k_lseek(fd, offset, whence);
#else
  errno = 0;
  off_t retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_LSEEK), "b"(fd), "c"(offset), "S"(whence));

  SYSCALL_SET_ERRNO(retval);

  return retval;
#endif
}
