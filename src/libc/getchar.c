#include <stdio.h>

#ifdef __is_libk
#include <arch/io.h>
#else
#include <unistd.h>
#endif

unsigned char getchar()
{
  unsigned char c;

#ifdef __is_libk
  c = arch_getchar(true);
#else
  do {
    read(STDIN_FILENO, &c, 1);
  } while (c == 0);
#endif

  return c;
}
