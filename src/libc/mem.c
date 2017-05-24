#include <mem.h>

void memcpy(void *src, void *dest, size_t bytes)
{
    char *s = (char *) src;
    char *d = (char *) dest;

    for(uint64_t i = 0; i < bytes; i++) {
        d[i] = s[i];
    }
}
