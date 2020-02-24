#include <string.h>

char* strcat(char* dest, const char* src)
{
  int i, j;

  for (i = 0; dest[i] != '\0'; i++)
    ;

  for (j = 0; src[j] != '\0'; j++) {
    dest[i + j] = src[j];
  }

  dest[i + j] = '\0';

  return dest;
}
