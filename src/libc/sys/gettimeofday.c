#include <sys/syscall.h>

int gettimeofday(struct timeval* p, void* z)
{
#ifdef __is_libk
  return k_gettimeofday(p, z);
#else
  int retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_GETTIMEOFDAY), "b"(p), "c"(z));

  return retval;
#endif
}
