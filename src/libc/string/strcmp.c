#include <string.h>

int strcmp(const char* s1, const char* s2)
{
  unsigned char c1, c2;

  while ((c1 = *s1++) == (c2 = *s2++)) {
    if (c1 == '\0') {
      return 0;
    }
  }

  return c1 - c2;
}
