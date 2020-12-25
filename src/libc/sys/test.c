#include <sys/syscall.h>

void test(const char* s)
{
#ifdef __is_libk
  k_test(s);
#else
  syscall(SYSCALL_TEST, s);
#endif
}
