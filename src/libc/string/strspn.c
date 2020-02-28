#include <string.h>

// This implementation comes from:
// https://clc-wiki.net/wiki/C_standard_library:string.h:strspn
size_t strspn(const char* s1, const char* s2)
{
  size_t ret = 0;

  while (*s1 && strchr(s2, *s1++)) {
    ret++;
  }

  return ret;
}
