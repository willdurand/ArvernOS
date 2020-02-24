#include <string.h>

int strcspn(const char* s1, const char* s2)
{
  const char* s = s1;
  const char* c;

  while (*s1) {
    for (c = s2; *c; c++) {
      if (*s1 == *c) {
        break;
      }
    }

    if (*c) {
      break;
    }

    s1++;
  }

  return s1 - s;
}
