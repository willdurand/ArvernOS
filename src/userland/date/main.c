#include <stdio.h>
#include <time.h>

int main()
{
  time_t now;
  char buf[40];

  time(&now);
  strftime(buf, 40, "%c", localtime(&now));

  printf("%s\n", buf);

  return 0;
}
