#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  char* s1 = malloc(sizeof(char) * 3);
  strcpy(s1, "s1");

  assert(strncmp(s1, "s1", 2) == 0, "returns 0 when strings are equal");
  assert(strncmp("", "", 0) == 0, "works for empty strings");
  assert(strncmp("a", "b", 1) != 0,
         "returns a non-zero value when strings are different");
  assert(strncmp("aa", "ab", 1) == 0, "compares the N first characters");

  free(s1);

  return test_summary();
}
