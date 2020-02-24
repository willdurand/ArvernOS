#include <string.h>

char* strncpy(char* dest, const char* src, size_t len)
{
  if (len-- > 0) {
    for (; len && (*dest++ = *src++); len--)
      ;
  }

  *dest = 0;

  return dest;
}
