#include <core/debug.h>
#include <string.h>

char* strsep(char** str, const char* sep)
{
  char *s = *str, *end;

  if (!s) {
    return 0;
  }

  end = s + strcspn(s, sep);

  if (*end) {
    *end++ = 0;
  } else {
    end = 0;
  }

  *str = end;
  return s;
}
