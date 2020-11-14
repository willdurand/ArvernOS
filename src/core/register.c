#include "register.h"

uint64_t read_cr0()
{
  uint64_t value;

  __asm__("mov %%cr0, %0" : "=r"(value) : /* no input */);

  return value;
}

uint64_t read_cr2()
{
  uint64_t value;

  __asm__("mov %%cr2, %0" : "=r"(value) : /* no input */);

  return value;
}

uint64_t read_cr3()
{
  uint64_t value;

  __asm__("mov %%cr3, %0" : "=r"(value) : /* no input */);

  return value;
}

void write_cr0(uint64_t value)
{
  __asm__("mov %0, %%cr0" : /* no output */ : "r"(value));
}

void write_cr3(uint64_t value)
{
  __asm__("mov %0, %%cr3" : /* no output */ : "r"(value));
}

uint64_t read_msr(uint64_t msr)
{
  uint32_t low, high;

  __asm__("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

  return (low | ((uint64_t)high << 32));
}

void write_msr(uint64_t msr, uint64_t value)
{
  uint32_t lo = (uint32_t)value;
  uint32_t hi = value >> 32;

  __asm__("wrmsr" : /* no output */ : "a"(lo), "d"(hi), "c"(msr));
}
