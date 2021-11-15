#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  char str[] = "foooo-bar-bazz";

  char* token = strtok(str, "-");
  assert(strlen(token) == 5, "returns the first token");
  assert(strncmp(token, "foooo", strlen(token)) == 0,
         "returns the first token (2)");

  int i = 0;
  int assertions = 0;
  while (token != NULL) {
    token = strtok(NULL, "-");
    i++;

    if (i == 1) {
      assert(strncmp(token, "bar", strlen(token)) == 0,
             "returns the second token");
      assertions++;
    } else if (i == 2) {
      assert(strncmp(token, "bazz", strlen(token)) == 0,
             "returns the third token");
      assertions++;
    } else if (i == 3) {
      assert(token == NULL, "returns NULL");
      assertions++;
    }
  }
  assert(assertions == 3, "found 2 more tokens and then NULL");

  return test_summary();
}
