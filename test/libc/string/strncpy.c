#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  const char* s1 = "this is s1";
  char* copy = malloc(sizeof(char) * (strlen(s1) + 1));
  char* ret = strncpy(copy, s1, strlen(s1) + 1);

  assert(strlen(copy) == strlen(s1), "has the same length");
  assert(strncmp(s1, copy, strlen(s1)) == 0,
         "copies the string to the first argument");
  assert(strncmp(ret, copy, strlen(ret)) == 0, "returns the copied string");

  free(copy);

  char* small = malloc(sizeof(char) * 4);
  strncpy(small, s1, 4);

  // This is 4 - 1 because the implementation ensures the string is
  // '\0'-terminated.
  assert(strlen(small) == 3, "respects the length passed as third argument");

  free(small);

  return test_summary();
}
