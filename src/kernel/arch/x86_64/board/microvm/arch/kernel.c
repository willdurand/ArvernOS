#include <arch/kernel.h>
#include <arvern/utils.h>

extern char __kernel_start;
extern char __kernel_end;

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
  return addr >= (uintptr_t)&__kernel_start && addr < (uintptr_t)&__kernel_end;
}
