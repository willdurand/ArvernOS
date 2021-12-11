#include <arch/io.h>

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
