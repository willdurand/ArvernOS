#include <sys/k_syscall.h>

#include <stdio.h>

int k_test(const char* s)
{
  printf("Hello, %s!\n", s);

  return 42;
}
