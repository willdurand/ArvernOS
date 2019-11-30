#include <string.h>

size_t strlen(const char* s) {
    size_t ret = 0;

    while (s[ret] != 0) {
        ret++;
    }

    return ret;
}

void reverse(char* s) {
    char c;

    for (size_t i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int strncmp(const char* s, const char* r, int len) {
    while (len-- && *s && *r) {
        if (*s != *r) {
            return *s - *r;
        }

        r++;
        s++;
    }

    if (len >= 0) {
        return *s - *r;
    }

    return 0;
}
