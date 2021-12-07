#include <arch/kshell.h>

#include <core/utils.h>
#include <stdint.h>
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

  // This is for testing purposes, it shouldn't work when we have usermode
  // because `syscall()` won't be available.
  extern int64_t syscall(uint64_t id, ...);
  const char* s = "syscall";
  syscall(1, s);

  return 0;
}
