#include "panic.h"
#include <core/isr.h>
#include <drivers/vga_text.h>
#include <stdio.h>

void kernel_panic(const char* format, ...)
{
  printf("\033[0;31m");

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  printf("\n\n%45s\033[0m", "SYSTEM HALTED!");

  vga_text_disable_cursor();
  irq_disable();

  while (1) {
    __asm__("hlt");
  }
}
