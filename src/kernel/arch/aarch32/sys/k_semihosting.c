#include "k_semihosting.h"

void k_semihosting(uint32_t operation, uint32_t parameters)
{
  register uint64_t r0 __asm__("r0") = operation;
  register uint64_t r1 __asm__("r1") = parameters;

  __asm__("svc #0x123456" ::"r"(r0), "r"(r1));
}
