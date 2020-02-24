#include <stdlib.h>
#include <string.h>

char* strdup(const char* s)
{
  size_t len = strlen(s) + 1;
  char* ret = malloc(len * sizeof(char));

  if (ret) {
    strncpy(ret, s, len);
  }

  return ret;
}
