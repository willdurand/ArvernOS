#include <test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    assert(strlen("s1") == 2, "returns the length of the string");
    assert(strlen("") == 0, "handles empty string");

    return test_summary();
}
