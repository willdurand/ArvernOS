#include <stdio.h>

#include <unistd.h>

void putchar(char c)
{
  write(STDOUT_FILENO, &c, 1);
}
