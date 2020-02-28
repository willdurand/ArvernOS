#include <string.h>

// This implementation comes from: https://clc-wiki.net/wiki/strchr
char* strchr(const char* s, int c)
{
  while (*s != (char)c) {
    if (!*s++) {
      return 0;
    }
  }

  return (char*)s;
}
