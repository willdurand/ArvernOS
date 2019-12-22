#include <test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    assert(strncmp("s1", "s1", 2) == 0, "returns 0 when strings are equal");
    assert(strncmp("", "", 0) == 0, "works for empty strings");
    assert(strncmp("a", "b", 1) != 0, "returns a non-zero value when strings are different");
    assert(strncmp("aa", "ab", 1) == 0, "compares the N first characters");

    return test_summary();
}
