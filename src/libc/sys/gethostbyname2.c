#include <sys/syscall.h>

int gethostbyname2(const char* name, struct in_addr* in)
{
#ifdef __is_libk
  return k_gethostbyname2(name, in);
#else
  int retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_GETHOSTBYNAME2), "S"(name), "d"(in));

  return retval;
#endif
}
