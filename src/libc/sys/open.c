#include <sys/syscall.h>

int open(const char* pathname, uint32_t flags)
{
#ifdef __is_libk
  return k_open(pathname, flags);
#else
  errno = 0;

  int retval = syscall(SYSCALL_OPEN, pathname, flags);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
