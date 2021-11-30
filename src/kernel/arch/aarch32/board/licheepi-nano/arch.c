#include <arch/io.h>
#include <arch/logging.h>

#include <arvern/utils.h>
#include <drivers/uart0.h>

unsigned char arch_getchar(bool blocking)
{
  UNUSED(blocking);

  return uart0_getc();
}

void arch_putchar(char c)
{
  uart0_putc(c);
}

void arch_logging_stream_output(char c, void* arg)
{
  UNUSED(*arg);

  uart0_putc(c);
}
