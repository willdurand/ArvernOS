#ifndef __is_libk
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* getenv(const char* name)
{
  int i = 0;
  while (environ[i] != NULL) {
    char* s = strchr(environ[i], '=');

    int pos = 0;
    if (s != NULL) {
      pos = s - environ[i];
    }

    if (pos > 0 && strncmp(environ[i], name, pos) == 0) {
      return environ[i] + pos + 1;
    }

    i++;
  }

  return NULL;
}

#endif
