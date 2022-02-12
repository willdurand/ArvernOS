#include <sys/syscall.h>

int openat(int dirfd, const char* pathname, int flags)
{
#ifdef __is_libk
  return k_openat(dirfd, pathname, flags);
#else
  int retval = syscall(SYSCALL_OPENAT, dirfd, pathname, flags);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
