#include <arch/panic.h>

#include <arvern/utils.h>
#include <stdio.h>

void arch_kernel_load_symbols(uint64_t addr, uint64_t size)
{
  UNUSED(addr);
  UNUSED(size);
}

void arch_kernel_dump_stacktrace()
{
  printf("(not implemented)\n");
}
