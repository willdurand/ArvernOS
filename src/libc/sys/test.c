#include <sys/syscall.h>

void test(const char* s)
{
  __asm__(INT_SYSCALL : /* no output */ : "a"(SYSCALL_TEST), "b"(s));
}
