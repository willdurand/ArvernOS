#include <stddef.h>
#include <string.h>

void* memmove(void* dest, const void* src, size_t len)
{
  unsigned char* d = dest;
  const unsigned char* s = src;

  if (d == s || len == 0) {
    return dest;
  }

  if (d < s) {
    while (len--) {
      *d++ = *s++;
    }
  } else {
    d += len;
    s += len;
    while (len--) {
      *--d = *--s;
    }
  }

  return dest;
}
