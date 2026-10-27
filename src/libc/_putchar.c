#include <stdio.h>

#ifdef __is_libk
#include <arch/io.h>

void _putchar(char c)
{
  arch_serial_putchar(c);
  arch_screen_putchar(c);
}

#else

void _putchar(char c)
{
  putchar(c);
}

#endif
