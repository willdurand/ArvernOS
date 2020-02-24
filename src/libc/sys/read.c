#include <sys/syscall.h>

char read()
{
  char c;

  __asm__(INT_SYSCALL : "=d"(c) : "a"(SYSCALL_READ));

  return c;
}
