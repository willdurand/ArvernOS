#include <sys/syscall.h>

void test(const char* s)
{
#ifdef __is_libk
  k_test(s);
#else
  __asm__(INT_SYSCALL : /* no output */ : "a"(SYSCALL_TEST), "b"(s));
#endif
}
