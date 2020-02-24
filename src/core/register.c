#include "register.h"

uint64_t read_cr2()
{
  uint64_t value;

  __asm__("mov %%cr2, %0" : "=r"(value) : /* no input */);

  return value;
}

void write_cr3(uint64_t value)
{
  __asm__("mov %0, %%cr3" : /* no output */ : "r"(value));
}

uint64_t read_cr3()
{
  uint64_t value;

  __asm__("mov %%cr3, %0" : "=r"(value) : /* no input */);

  return value;
}
