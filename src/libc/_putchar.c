#include <stdio.h>

#ifdef __is_libk
#include <drivers/vga_text.h>

void _putchar(char c)
{
  vga_text_write(c);
}

#else

void _putchar(char c)
{
  putchar(c);
}

#endif
