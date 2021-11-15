#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  char* string = strdup("ho hoo hoooo");
  char* original_string = string;
  char* found;

  found = strsep(&string, " ");
  assert(strlen(found) == 2, "returns the first part");

  found = strsep(&string, " ");
  assert(strlen(found) == 3, "returns the second part");

  found = strsep(&string, " ");
  assert(strlen(found) == 5, "returns the last part");

  found = strsep(&string, " ");
  assert(found == 0, "returns 0 when there is no more part");

  free(original_string);

  return test_summary();
}
