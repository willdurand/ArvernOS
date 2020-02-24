#include <string.h>

void* memset(void* str, int c, size_t bytes)
{
  for (size_t i = 0; i < bytes; i++) {
    ((unsigned char*)str)[i] = (unsigned char)c;
  }

  return str;
}
