#include <arch/kernel.h>

#include <arvern/utils.h>

void arch_restart()
{
  // TODO: Implement this function for aarch32.
}

void arch_poweroff(int exit_code)
{
  UNUSED(exit_code);
  // TODO: Implement this function for aarch32.
}

void arch_halt()
{
  while (1) {
    __asm__("nop");
  }
}
