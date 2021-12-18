#include <arch/kernel.h>

#include <core/isr.h>
#include <core/port.h>
#include <drivers/vga_text.h>

#define KEYBOARD_STATUS_PORT 0x64
#define RESET_CPU_COMMAND    0xFE

void arch_halt()
{
  vga_text_disable_cursor();
  isr_disable_interrupts();

  while (1) {
    // This allows the CPU to enter a sleep state in which it consumes much less
    // energy. See: https://en.wikipedia.org/wiki/HLT_(x86_instruction)
    __asm__("hlt");
  }
}

void arch_restart()
{
  isr_disable_interrupts();

  uint8_t status = 0x02;
  while (status & 0x02) {
    status = port_byte_in(KEYBOARD_STATUS_PORT);
  }

  port_byte_out(KEYBOARD_STATUS_PORT, RESET_CPU_COMMAND);
}

void arch_poweroff(int exit_code)
{
  isr_disable_interrupts();

#ifdef CONFIG_SEMIHOSTING
  if (exit_code == 0) {
    // Power-off for QEMU, see: https://wiki.osdev.org/Shutdown
    port_word_out(0x604, 0x2000);
  } else {
    port_byte_out(0x501, exit_code);
  }
#endif
}
