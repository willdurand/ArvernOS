#include <stdlib.h>
#include <string.h>

int atoi(char* s)
{
  int value = 0;
  int sign = 1;

  int i = 0;
  while (s[i] && s[i] == ' ') {
    i++;
  }

  if (s[i] == '-') {
    sign = -1;
    i++;
  }

  while (s[i]) {
    char c = s[i];
    if (c < '0' || c > '9') {
      break;
    }

    value = value * 10;
    value += (c - '0');

    i++;
  }

  // TODO: Handle min/max values.

  return sign * value;
}
