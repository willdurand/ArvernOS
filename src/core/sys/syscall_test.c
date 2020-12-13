#include "syscall.h"
#include <stdio.h>

void syscall_test(registers_t* registers)
{
  printf("  (syscall_test) hello, %s!\n", registers->rbx);
}
