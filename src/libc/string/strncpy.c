#include <string.h>

char* strncpy(char* dest, const char* src, size_t len)
{
  if (len == 0) {
    return dest;
  }

  size_t i = 0;
  while (i < len - 1 && src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }

  dest[i] = '\0';

  return dest;
}
