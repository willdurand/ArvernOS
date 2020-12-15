#include "syscall.h"
#include <stdio.h>

void k_test(const char* s)
{
  printf("  (syscall_test) hello, %s!\n", s);
}
