#include <stdio.h>

#ifdef __is_libk
#include <arch/io.h>
#else
#include <unistd.h>
#endif

unsigned char getchar()
{
  unsigned char c = 0;

#ifdef __is_libk
  c = arch_getchar(true);
#else
  ssize_t n;

  do {
    n = read(STDIN_FILENO, &c, 1);
  } while (n > 0 && c == 0);
#endif

  return c;
}
