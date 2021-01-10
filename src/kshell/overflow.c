#include "kshell.h"
#include <string.h>

void overflow()
{
  char c[12];
  strcpy(c, "123456789012345678901234567890");
}
