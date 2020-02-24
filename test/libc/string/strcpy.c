#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  const char* s1 = "this is s1";
  char* copy = malloc(sizeof(char) * (strlen(s1) + 1));
  char* ret = strcpy(copy, s1);

  assert(strlen(copy) == strlen(s1), "has the same length");
  assert(strncmp(s1, copy, strlen(s1)) == 0,
         "copies the string to the first argument");
  assert(strncmp(ret, copy, strlen(ret)) == 0, "returns the copied string");

  free(copy);

  return test_summary();
}
