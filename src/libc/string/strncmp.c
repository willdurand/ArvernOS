#include <string.h>

int strncmp(const char* s, const char* r, int len)
{
  while (len-- && *s && *r) {
    if (*s != *r) {
      return *s - *r;
    }

    r++;
    s++;
  }

  if (len >= 0) {
    return *s - *r;
  }

  return 0;
}
