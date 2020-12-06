#include <string.h>

void* memchr(const void* str, int c, size_t n)
{
  unsigned char* ptr = (unsigned char*)str;
  for (size_t i = 0; i < n; i++, ptr++) {
    if (*ptr == (unsigned char)c) {
      return ptr;
    }
  }

  return NULL;
}
