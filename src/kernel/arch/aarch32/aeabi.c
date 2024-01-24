#include <string.h>

// Although we are linking to libgcc, some functions generated w.r.t. the ARM
// Embedded ABI are not available apparently. This is why this file exists and
// the functions defined below should basically be calling functions already
// implemented in this project.
//
// See:
// https://stackoverflow.com/questions/31494087/linking-rust-with-c-undefined-reference-to-aeabi-functions

void __aeabi_memclr(void* dest, size_t n)
{
  memset(dest, 0, n);
}

void __aeabi_memclr4(void* dest, size_t n)
{
  memset(dest, 0, n);
}
