#include <ctype.h>
#include <stdio.h>
#include <test.h>

int main()
{
  for (int i = 0; i < 10; i++) {
    assert(isdigit('0' + i), "returns true when char is a digit");
  }

  assert(!isdigit('a'), "returns false when char is not a digit");

  return test_summary();
}
