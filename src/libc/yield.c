#include <sys/syscall.h>

int yield()
{
#ifdef __is_libk
  return k_yield();
#else
  return syscall(SYSCALL_YIELD);
#endif
}
