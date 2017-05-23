#include <string.h>

int strlen(const char* s)
{
    int ret = 0;
    while (s[ret] != 0) {
        ret++;
    }

    return ret;
}

void reverse(char* s)
{
    char c;
    for (int i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
