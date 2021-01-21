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

  kernel_dump_stacktrace();

  printf("\n\n%45s\033[0m", "SYSTEM HALTED!");

  vga_text_disable_cursor();
  isr_disable_interrupts();

  while (1) {
    __asm__("hlt");
  }
}

void kernel_dump_stacktrace()
{
  printf("\n  Kernel stacktrace:\n\n");
  DEBUG("%s", "kernel stacktrace:");

  stack_frame_t* stackframe = NULL;
  __asm__("movq %%rbp, %0" : "=r"(stackframe));

  while (stackframe != NULL) {
    uint64_t address = stackframe->rip;

    printf("     %p\n", address);
    DEBUG("  %p", address);

    stackframe = stackframe->rbp;
  }
}
