#include <sys/syscall.h>

#include <fcntl.h>

int open(const char* pathname, uint32_t flags)
{
#ifdef __is_libk
  return k_open(pathname, flags);
#else
  int retval;

#ifdef __aarch64__
  retval = syscall(SYSCALL_OPENAT, AT_FDCWD, pathname, flags);
#else
  retval = syscall(SYSCALL_OPEN, pathname, flags);
#endif

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
