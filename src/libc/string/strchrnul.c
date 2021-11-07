#include <string.h>

char* strchrnul(const char* s, int c)
{
  char* occurrence = strchr(s, c);

  if (occurrence) {
    return occurrence;
  }

  return (char*)s + strlen(s);
}
