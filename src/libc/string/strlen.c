#include <string.h>

size_t strlen(const char* s)
{
  const char* str;

  if (!s) {
    return 0;
  }

  for (str = s; *str; str++)
    ;

  return (str - s);
}
