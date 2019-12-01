#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

void* memcpy(const void* src, void* dest, size_t bytes);
void* memset(void* str, int c, size_t bytes);
void* memmove(void* dest, const void* src, size_t len);

#endif
