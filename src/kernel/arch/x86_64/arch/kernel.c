#include <arch/kernel.h>

#include <core/isr.h>
#include <drivers/vga_text.h>

void arch_halt_system()
{
  vga_text_disable_cursor();
  isr_disable_interrupts();

  while (1) {
    // This allows the CPU to enter a sleep state in which it consumes much less
    // energy. See: https://en.wikipedia.org/wiki/HLT_(x86_instruction)
    __asm__("hlt");
  }
}
