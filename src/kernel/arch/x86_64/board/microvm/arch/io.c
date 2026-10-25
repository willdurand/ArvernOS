#include <arch/io.h>
#include <arvern/utils.h>

unsigned char arch_getchar(bool blocking)
{
  UNUSED(blocking);

  return 0;
}

void arch_putchar(char c)
{
  UNUSED(c);
}
