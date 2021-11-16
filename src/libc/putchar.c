#include <stdio.h>

#include <unistd.h>

void putchar(char c)
{
  // Make sure we write a null-terminated string.
  write(STDOUT_FILENO, (char[]){ c, '\0' }, 1);
}
