#include <sys/syscall.h>

off_t lseek(int fd, off_t offset, int whence)
{
#ifdef __is_libk
  return k_lseek(fd, offset, whence);
#else
  off_t ret;

  __asm__(INT_SYSCALL
          : "=d"(ret)
          : "a"(SYSCALL_LSEEK), "b"(fd), "c"(offset), "S"(whence));

  return ret;
#endif
}
