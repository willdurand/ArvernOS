#include <arch/kernel.h>

#include <stdint.h>
#include <sys/k_semihosting.h>

#define SYS_EXIT 0x18

void arch_restart()
{
  // TODO: Implement this function for AArch64.
}

void arch_poweroff(int exit_code)
{
#ifdef CONFIG_SEMIHOSTING
  // See:
  // https://developer.arm.com/documentation/dui0205/f/semihosting/debug-agent-interaction-swis/angel-swireason-reportexception--0x18-
  uint64_t parameters[] = {
    0x20026,  // ADP_Stopped_ApplicationExit
    exit_code // exit code
  };

  k_semihosting(SYS_EXIT, (uint64_t)parameters);
#endif
}

void arch_halt()
{
  while (1) {
    __asm__("wfi");
  }
}
