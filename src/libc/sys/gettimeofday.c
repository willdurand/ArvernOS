#include <sys/syscall.h>

int gettimeofday(struct timeval* p, void* z)
{
  __asm__(INT_SYSCALL
          : /* no output */
          : "a"(SYSCALL_GETTIMEOFDAY), "b"(p), "c"(z));

  return 0;
}
