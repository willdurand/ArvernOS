#include "init.h"
#include <string.h>

int overflow()
{
  char c[12];
  strcpy(c, "123456789012345678901234567890");
  return 1;
}
