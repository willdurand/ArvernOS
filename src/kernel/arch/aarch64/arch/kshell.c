#include <arch/kshell.h>

#include <core/utils.h>
#include <stdio.h>

int arch_selftest()
{
  int exception_level = get_el();

  if (exception_level == 1) {
    printf("Got expected exception level: %d\n", exception_level);
  } else {
    printf("Got unexpected exception level: %d (expected: 1)\n",
           exception_level);
    return 1;
  }

  return 0;
}
