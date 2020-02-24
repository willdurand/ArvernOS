#include "panic.h"
#include <core/isr.h>
#include <drivers/screen.h>
#include <stdio.h>

void kernel_panic(const char* format, ...)
{
  screen_color_scheme(COLOR_RED, COLOR_BLACK);

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  printf("\n\n%45s", "SYSTEM HALTED!");

  irq_disable();

  while (1) {
    __asm__("hlt");
  }
}
