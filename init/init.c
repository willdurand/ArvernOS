#include <stdio.h>

int main() {
    for (int i = 0; i < 3; i++) {
        printf("(%d) please press a key: %c\n", i, getchar());
    }

    return 0;
}
