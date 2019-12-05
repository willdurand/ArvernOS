#include <string.h>

void reverse(char* s) {
    char c;

    for (size_t i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
