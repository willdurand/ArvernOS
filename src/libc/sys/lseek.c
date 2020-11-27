#include <sys/syscall.h>

off_t lseek(int fd, off_t offset, int whence)
{
  off_t ret;

  __asm__(INT_SYSCALL
          : "=d"(ret)
          : "a"(SYSCALL_LSEEK), "b"(fd), "c"(offset), "S"(whence));

  return ret;
}
