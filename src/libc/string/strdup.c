#include <string.h>
#include <stdlib.h>

char* strdup(const char* s) {
    size_t size = strlen(s) + 1;
    char* ret = malloc(size * sizeof(char));

    if (ret) {
        strcpy(ret, s);
    }

    return ret;
}
