#include <arch/kernel.h>
#include <stdint.h>

#define SYS_EXIT 0x18

void semihosting_call(uint64_t operation, uint64_t parameters);

void arch_restart()
{
  // TODO:
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

  semihosting_call(SYS_EXIT, (uint64_t)parameters);
#endif
}

void arch_halt()
{
  // TODO:

  while (1) {
    ;
  }
}

void semihosting_call(uint64_t operation, uint64_t parameters)
{
  register uint64_t x0 __asm__("x0") = operation;
  register uint64_t x1 __asm__("x1") = parameters;

  __asm__("hlt #0xF000" ::"r"(x0), "r"(x1));
}
