#include <string.h>
#include <stdlib.h>

void itoa(int n, char* str, int base)
{
    int i = 0;
    int sign;

    if (n == 0) {
        str[i++] = '0';
        str[i] = '\0';

        return;
    }

    if ((sign = n) < 0 && base == 10) {
        n = -n;
    }

    do {
        int r = n % base;
        str[i++] = (r < 10) ? r + '0' : r + 'a' - 10;
    } while ((n /= base) > 0);

    if (sign < 0 && base == 10) {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse(str);
}

void ulltoa(uint64_t n, char* str, int base)
{
    int i = 0;
    int sign;

    if (n == 0) {
        str[i++] = '0';
        str[i] = '\0';

        return;
    }

    if ((sign = n) < 0 && base == 10) {
        n = -n;
    }

    do {
        int r = n % base;
        str[i++] = (r < 10) ? r + '0' : r + 'a' - 10;
    } while ((n /= base) > 0);

    if (sign < 0 && base == 10) {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse(str);
}
