#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  char* foo = malloc(sizeof(char) * 4);
  strcpy(foo, "foo");

  assert(strcmp(foo, "foo") == 0, "returns 0 when the two strings are equal");
  assert(strcmp(foo, "fo") > 0,
         "returns a positive number when the first string is greater");
  assert(strcmp("fo", foo) < 0,
         "returns a negative number when the first string is smaller");
  assert(strcmp("bar", foo) != 0,
         "returns non-zero when the two strings are not equal");
  assert(strcmp(foo, "bar") != 0,
         "returns non-zero when the two strings are not equal (2)");

  free(foo);

  return test_summary();
}
