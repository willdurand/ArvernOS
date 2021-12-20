#include <arch/kshell.h>

#include <core/utils.h>
#include <stdio.h>

int arch_selftest()
{
  int exception_level = get_el();

  if (exception_level == 0x13) {
    printf("Got expected exception level: 0x%x\n", exception_level);
  } else {
    printf("Got unexpected exception level: 0x%x (expected: 0x13)\n",
           exception_level);
    return 1;
  }

  // This is for testing purposes, it shouldn't work when we have usermode
  // because `syscall()` won't be available.
  extern int syscall(int id, ...);
  const char* s = "syscall";
  syscall(1, s);

  return 0;
}
