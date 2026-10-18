#include <arch/kernel.h>
#include <arvern/utils.h>

void arch_restart()
{
  // TODO: implement me
}

void arch_poweroff(int exit_code)
{
  // TODO: implement me
  UNUSED(exit_code);
}

void arch_halt()
{
  // TODO: implement me
}

bool arch_is_kernel_address(uintptr_t addr)
{
  UNUSED(addr);

  return false;
}
