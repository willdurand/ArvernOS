#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char* s);

int strcmp(const char* s1, const char* s2);

int strncmp(const char* s, const char* r, int len);

char* strcpy(char* dest, const char* src);

/// This is actually a safer implementation based on `strlcpy()`.
char* strncpy(char* dest, const char* src, size_t len);

char* strsep(char** str, const char* sep);

int strcspn(const char* s1, const char* s2);

char* strdup(const char* s);

char* strcat(char* dest, const char* src);

void* memcpy(void* dest, const void* src, size_t bytes);

void* memset(void* str, int c, size_t bytes);

void* memmove(void* dest, const void* src, size_t len);

char* strtok(char* str, const char* delim);

size_t strspn(const char* s1, const char* s2);

char* strchr(const char* s, int c);

#endif
