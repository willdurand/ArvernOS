#include <arch/io.h>

#include <arvern/utils.h>
#include <drivers/miniuart.h>

void arch_putchar(char c)
{
  miniuart_putc(c);
}

unsigned char arch_getchar(bool blocking)
{
  UNUSED(blocking);

  return miniuart_getc();
}
