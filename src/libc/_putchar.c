#include <stdio.h>

#ifdef __is_libk
#include <console/console.h>

void _putchar(char c)
{
  console_putchar(c);
}

#else

void _putchar(char c)
{
  putchar(c);
}

#endif
