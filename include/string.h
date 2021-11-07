#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// This is needed because `strdup()` relies on `malloc()`, which isn't
// available in the `libc` yet.
#ifdef __is_libk

char* strdup(const char* s);

#endif

size_t strlen(const char* s);

int strcmp(const char* s1, const char* s2);

int strncmp(const char* s, const char* r, int len);

char* strcpy(char* dest, const char* src);

/// This is actually a safer implementation based on `strlcpy()`.
char* strncpy(char* dest, const char* src, size_t len);

char* strsep(char** str, const char* sep);

int strcspn(const char* s1, const char* s2);

char* strcat(char* dest, const char* src);

void* memcpy(void* dest, const void* src, size_t bytes);

void* memset(void* str, int c, size_t bytes);

void* memmove(void* dest, const void* src, size_t len);

char* strtok(char* str, const char* delim);

size_t strspn(const char* s1, const char* s2);

char* strchr(const char* s, int c);

// Like strchr() except that when `c` is not found in `s`, it returns a pointer
// to the null byte at the end of `s`, rather than `NULL`.
char* strchrnul(const char* s, int c);

// Scans the initial `n` bytes of the memory area pointed to by `str` for the
// first instance of `c`. Both `c` and the bytes of the memory area pointed to
// by `str` are interpreted as unsigned char.
void* memchr(const void* str, int c, size_t n);

// Compares the first `length` bytes of memory area `a_ptr` and memory area
// `b_ptr`.
int memcmp(const void* a_ptr, const void* b_ptr, size_t length);

#endif
