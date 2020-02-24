#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  const char* str1 = "123456789a";
  const char* str2 = "a";
  const char* str3 = "123";

  assert(strcspn(str1, str2) == 9,
         "returns the number of chars in str1 that are not in str2");
  assert(strcspn(str1, str3) == 0, "returns 0 when s2 in a substring of s1");
  assert(strcspn(str1, str1) == 0, "returns 0 when s1 == s2");

  return test_summary();
}
