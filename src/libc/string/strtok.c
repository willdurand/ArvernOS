#include <string.h>

// This implementation comes from:
// https://clc-wiki.net/wiki/C_standard_library:string.h:strtok
char* strtok(char* str, const char* delim)
{
  static char* p = 0;

  if (str) {
    p = str;
  } else if (!p) {
    return 0;
  }

  str = p + strspn(p, delim);
  p = str + strcspn(str, delim);

  if (p == str) {
    return p = 0;
  }

  p = *p ? * p = 0, p + 1 : 0;

  return str;
}
