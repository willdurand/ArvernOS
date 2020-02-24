#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  const char* s1 = "s1";
  const char* s2 = "s2";
  char* s = malloc(5 * sizeof(char));
  strcpy(s, "");

  strcat(s, s1);
  assert(strcmp(s, "s1") == 0, "appends the string src to dest");
  strcat(s, s2);
  assert(strcmp(s, "s1s2") == 0, "appends the string src to dest");

  free(s);

  return test_summary();
}
