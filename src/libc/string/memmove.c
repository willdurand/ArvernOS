#include <string.h>

void* memmove(void* dest, const void* src, size_t len)
{
  unsigned char cpy[len];
  memcpy(cpy, src, len);
  return memcpy(dest, cpy, len);
}
