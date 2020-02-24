#include <string.h>

void* memcpy(void* dest, const void* src, size_t bytes)
{
  const char* s = (char*)src;
  char* d = (char*)dest;

  for (size_t i = 0; i < bytes; i++) {
    d[i] = s[i];
  }

  return dest;
}
