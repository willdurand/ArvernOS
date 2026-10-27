#include <arch/io.h>

#include <arvern/utils.h>
#include <drivers/serial.h>

unsigned char arch_getchar(bool blocking)
{
  if (!blocking && !serial_is_waiting(SERIAL_COM1)) {
    return 0;
  }

  return serial_read(SERIAL_COM1);
}

void arch_serial_putchar(char c)
{
  serial_write(SERIAL_COM1, c);
}

void arch_screen_putchar(char c)
{
  UNUSED(c);
}
