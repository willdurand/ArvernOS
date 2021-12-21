#ifndef __is_libk
#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__)) void abort()
{
  // TODO: Abnormally terminate the process as if by SIGABRT.
  printf("abort() was called\n");

  while (1) {
    ;
  }
  __builtin_unreachable();
}

#endif
