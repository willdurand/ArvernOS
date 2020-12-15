#include <sys/syscall.h>

int open(const char* pathname, uint32_t flags)
{
#ifdef __is_libk
  return k_open(pathname, flags);
#else
  errno = 0;
  int retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_OPEN), "b"(pathname), "c"(flags));

  SYSCALL_SET_ERRNO(retval);

  return retval;
#endif
}
