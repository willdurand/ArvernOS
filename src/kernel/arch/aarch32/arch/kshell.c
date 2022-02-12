#include <arch/kshell.h>

#include <core/utils.h>
#include <stdio.h>
#include <sys/syscall.h>

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
  int retval = syscall(SYSCALL_TEST, s);

  if (retval == 42) {
    printf("Got expected syscall return value: %d\n", retval);
  } else {
    printf("Got unexpected syscall return value: %d (expected: 42)\n", retval);
    return 1;
  }

  return 0;
}
