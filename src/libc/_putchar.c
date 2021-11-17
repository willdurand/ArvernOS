#include <stdio.h>

#ifdef __is_libk
#include <arch/io.h>

void _putchar(char c)
{
  arch_putchar(c);
}

#else

void _putchar(char c)
{
  putchar(c);
}

#endif
