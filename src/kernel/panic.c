#include <panic.h>

#include <arch/kernel.h>
#include <stdio.h>

void arch_kernel_panic(bool dump_stacktrace, const char* format, ...)
{
  printf("\033[0;31m");

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  if (dump_stacktrace) {
    arch_kernel_dump_stacktrace();
  }

  printf("\n\n%45s\033[0m", "SYSTEM HALTED!");

  arch_halt_system();
}
