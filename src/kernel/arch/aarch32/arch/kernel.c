#include <arch/kernel.h>

void arch_restart()
{
  // TODO:
}

void arch_poweroff()
{
  // TODO:
}

void arch_halt()
{
  while (1) {
    __asm__("nop");
  }
}
