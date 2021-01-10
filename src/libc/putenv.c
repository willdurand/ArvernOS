#ifndef __is_libk
#include <stdlib.h>
#include <unistd.h>

int putenv(char* str)
{
  for (int i = 0; i < 10; i++) {
    if (environ[i] == NULL) {
      environ[i] = str;
      return 0;
    }
  }

  return -1;
}

#endif
