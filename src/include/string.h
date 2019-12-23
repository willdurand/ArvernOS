#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char* s);

int strncmp(const char* s, const char* r, int len);

char* strcpy(char* dest, const char* src);

char* strsep(char** str, const char* sep);

int strcspn(const char* s1, const char* s2);

char* strdup(const char* s);

void* memcpy(void* dest, const void* src, size_t bytes);

void* memset(void* str, int c, size_t bytes);

void* memmove(void* dest, const void* src, size_t len);

#endif
