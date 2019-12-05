#include <string.h>

size_t strlen(const char* s) {
    size_t ret = 0;

    while (s[ret] != 0) {
        ret++;
    }

    return ret;
}
