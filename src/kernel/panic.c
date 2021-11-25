#include <panic.h>

#include <arch/kernel.h>
#include <arch/panic.h>
#include <logging.h>
#include <stdio.h>

void kernel_panic(bool dump_stacktrace, const char* format, ...)
{
  printf("\033[0;31m");

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  FATAL(format, args);

  if (dump_stacktrace) {
    arch_kernel_dump_stacktrace();
  }

  printf("\n\n%45s\033[0m", "SYSTEM HALTED!");

  arch_halt();
}
