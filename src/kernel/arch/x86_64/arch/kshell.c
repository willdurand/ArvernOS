#include <arch/kshell.h>

#include <stdio.h>

void arch_selftest()
{
  printf("interrupts: invoking breakpoint exception\n");
  __asm__("int3");
}
