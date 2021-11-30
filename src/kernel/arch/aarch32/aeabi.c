#include <string.h>

void __aeabi_memclr(void* dest, size_t n)
{
  memset(dest, 0, n);
}

void __aeabi_memclr4(void* dest, size_t n)
{
  memset(dest, 0, n);
}
