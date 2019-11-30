#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char* s);
void reverse(char* s);
int strncmp(const char* s, const char* r, int len);
char* strcpy(char* dest, const char* src);

#endif
