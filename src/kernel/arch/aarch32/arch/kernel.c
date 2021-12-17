#include <arch/kernel.h>

#include <arvern/utils.h>
#include <sys/k_semihosting.h>

void arch_restart()
{
  // TODO: Implement this function for AArch32.
}

void arch_poweroff(int exit_code)
{
#ifdef CONFIG_SEMIHOSTING
  uint32_t parameters[] = {
    0x20026,  // ADP_Stopped_ApplicationExit
    exit_code // exit code
  };

  // We use SYS_EXIT_EXTENDED because SYS_EXIT does not allow to pass an exit
  // code on AArch32.
  k_semihosting(SYS_EXIT_EXTENDED, (uint32_t)parameters);
#else
  UNUSED(exit_code);
#endif
}

void arch_halt()
{
  while (1) {
    __asm__("nop");
  }
}
