#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  char str[] = "My name is Will";
  char* ch = strchr(str, 'a');
  assert((ch - str + 1) == 5, "returns the part of str starting with the char");

  ch = strchr(str, 'x');
  assert(ch == NULL, "returns null when the char is not found");

  return test_summary();
}
