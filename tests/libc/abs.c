#include <stdio.h>
#include <stdlib.h>
#include <test.h>

int main()
{
  assert(abs(1) == 1, "handles strictly positive integers");
  assert(abs(-1) == 1, "handles strictly negative integers");
  assert(abs(0) == 0, "handles 0");

  return test_summary();
}
