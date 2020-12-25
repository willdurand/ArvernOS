#include <sys/syscall.h>

int gettimeofday(struct timeval* p, void* z)
{
#ifdef __is_libk
  return k_gettimeofday(p, z);
#else
  return syscall(SYSCALL_GETTIMEOFDAY, p, z);
#endif
}
