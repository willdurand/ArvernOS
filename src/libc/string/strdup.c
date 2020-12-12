#include <stdlib.h>
#include <string.h>

// This is needed because `strdup()` relies on `malloc()`, which isn't
// available in the `libc` yet.
#ifdef __is_libk

char* strdup(const char* s)
{
  size_t len = strlen(s) + 1;
  char* ret = malloc(len * sizeof(char));

  if (ret) {
    strncpy(ret, s, len);
  }

  return ret;
}

#endif
