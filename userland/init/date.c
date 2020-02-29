#include "init.h"
#include <stdio.h>
#include <time.h>

void date()
{
  time_t now;
  char buf[40];

  time(&now);
  strftime(buf, 40, "%c", localtime(&now));

  printf("%s\n", buf);
}
