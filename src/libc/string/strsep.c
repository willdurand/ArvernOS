#include <string.h>
#include <core/debug.h>

char* strsep(char** stringp, const char* delim) {
    char* begin = *stringp;

    if (*begin == '\0') {
        return 0;
    }

    char* end = begin + strcspn(begin, delim);

    if (*end) {
        *end++ = '\0';
        *stringp = end;
    } else {
        *stringp = 0;
    }

    return begin;
}
