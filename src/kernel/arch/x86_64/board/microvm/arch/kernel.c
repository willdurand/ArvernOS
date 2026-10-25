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
#ifdef CONFIG_SEMIHOSTING
  // Generate a triple-fault to trigger a guest-initiated shutdown, see:
  // https://www.qemu.org/docs/master/system/i386/microvm.html#triggering-a-guest-initiated-shut-down

  // Structure matching an IDT register.
  struct
  {
    uint16_t limit;
    uint64_t base;
  } __attribute__((packed)) idt_reg = { 0, 0 };

  // Load the empty IDT and trigger an interrupt.
  __asm__ volatile("lidt %0;"
                   "int $3;"
                   :
                   : "m"(idt_reg));
#endif

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
