#include <stdio.h>

#ifdef __is_libk
#include <drivers/screen.h>

void _putchar(char c)
{
  screen_write(c);
}

#else

void _putchar(char c)
{
  putchar(c);
}

#endif
