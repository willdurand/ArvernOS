#include <arch/kernel.h>

#include <core/isr.h>
#include <drivers/vga_text.h>

void arch_halt_system()
{
  vga_text_disable_cursor();
  isr_disable_interrupts();

  while (1) {
    __asm__("hlt");
  }
}
