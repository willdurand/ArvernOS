#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  const char* s1 = "this is s1";
  char* s2 = strdup(s1);

  assert(strncmp(s1, s2, strlen(s1)) == 0, "duplicates string");
  assert(strlen(s1) == strlen(s2), "has similar size");

  free(s2);

  return test_summary();
}
