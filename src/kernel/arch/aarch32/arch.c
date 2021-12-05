#include <arch/kernel.h>
#include <arch/kshell.h>
#include <arch/panic.h>
#include <arch/time.h>

#include <arvern/utils.h>

void arch_gettimeofday(struct timeval* p, void* z)
{
  UNUSED(*z);

  // TODO:

  p->tv_sec = 0;
  p->tv_usec = 0;
}

void arch_kernel_load_symbols(uint64_t addr, uint64_t size)
{
  UNUSED(addr);
  UNUSED(size);

  // TODO:
}

void arch_kernel_dump_stacktrace()
{
  // TODO:
}

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

void arch_selftest()
{
  // TODO:
}
