#include "k_semihosting.h"

void k_semihosting(uint64_t operation, uint64_t parameters)
{
  register uint64_t x0 __asm__("x0") = operation;
  register uint64_t x1 __asm__("x1") = parameters;

  __asm__("hlt #0xF000" ::"r"(x0), "r"(x1));
}
