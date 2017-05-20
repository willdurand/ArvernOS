#include <string.h>
#include "stdlib.h"

void itoa(int n, char str[]) {
    int i = 0;
    int sign;

    if ((sign = n) < 0) {
        n = -n;
    }

    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        str[i++] = '-';
    }
    str[i] = '\0';

    reverse(str);
}
