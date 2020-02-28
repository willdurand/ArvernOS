#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  int len = strspn("abc def", "xyz");
  assert(len == 0, "returns 0 when there is no match");

  len = strspn("abc def", "ab");
  assert(len == 2, "returns a length");

  return test_summary();
}
