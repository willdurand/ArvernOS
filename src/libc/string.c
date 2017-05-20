#include <string.h>

int strlen(const char* str)
{
    int ret = 0;
    while (str[ret] != 0)
        ret++;

    return ret;
}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
