#include <sys/syscall.h>

int test(const char* s)
{
#ifdef __is_libk
  return k_test(s);
#else
  return syscall(SYSCALL_TEST, s);
#endif
}
