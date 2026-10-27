#include <arch/io.h>

#include <arvern/utils.h>
#include <drivers/miniuart.h>

unsigned char arch_getchar(bool blocking)
{
  UNUSED(blocking);

  return miniuart_getc();
}

void arch_screen_putchar(char c)
{
  UNUSED(c);
}

void arch_serial_putchar(char c)
{
  miniuart_putc(c);
}
