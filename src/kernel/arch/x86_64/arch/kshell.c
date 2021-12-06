#include <arch/kshell.h>

#include <stdio.h>

int arch_selftest()
{
  printf("interrupts: invoking breakpoint exception\n");
  __asm__("int3");

  return 0;
}
