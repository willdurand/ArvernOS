#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  assert(strcmp("foo", "foo") == 0, "returns 0 when the two strings are equal");
  assert(strcmp("foo", "fo") == 1,
         "returns non-zero when the two strings are equal");
  assert(strcmp("fo", "foo") == -1,
         "returns non-zero when the two strings are equal (2)");

  return test_summary();
}
