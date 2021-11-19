#include <sys/k_syscall.h>

#include <stdio.h>

void k_test(const char* s)
{
  printf("Hello, %s!\n", s);
}
