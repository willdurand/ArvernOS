#include <arch/kernel.h>

#include <arvern/utils.h>
#include <core/isr.h>

void arch_halt()
{
  isr_disable_interrupts();

  while (1) {
    // This allows the CPU to enter a sleep state in which it consumes much less
    // energy. See: https://en.wikipedia.org/wiki/HLT_(x86_instruction)
    __asm__("hlt");
  }
}

void arch_restart()
{
  // There is no way to restart this board.
  arch_poweroff(0);
}

void arch_poweroff(int exit_code)
{
  UNUSED(exit_code);

  isr_disable_interrupts();

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

  arch_halt();
}
