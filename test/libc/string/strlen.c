#include <test.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *s1 = malloc(sizeof(char) * 2);
    strcpy(s1, "s1");

    assert(strlen(s1) == 2, "returns the length of the string");
    assert(strlen("") == 0, "handles empty string");

    free(s1);

    return test_summary();
}
