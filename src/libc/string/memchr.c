#include <stdint.h>
#include <string.h>

void* memchr(const void* str, int c, size_t n)
{
  uint8_t* ptr = (uint8_t*)str;
  uint8_t c_chr = (uint8_t)c;

  for (size_t i = 0; i < n; i++, ptr++) {
    if (*ptr == c_chr) {
      return ptr;
    }
  }

  return NULL;
}
