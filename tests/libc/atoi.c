#include <stdio.h>
#include <stdlib.h>
#include <test.h>

int main()
{
  assert(atoi("123") == 123, "handles strictly positive numbers");
  assert(atoi("   123") == 123, "handles leading spaces");
  assert(atoi("123a") == 123, "handles non-digits");
  assert(atoi("-123") == -123, "handles signed numbers");
  assert(atoi("0") == 0, "handles zero");
  assert(atoi("-0") == 0, "handles signed zero");
  assert(atoi("-1.2") == -1, "ignores floating points");

  return test_summary();
}
