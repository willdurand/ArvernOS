#include "mmio.h"

uint32_t mmio_read(uintptr_t reg)
{
  return *(uint32_t*)reg;
}

void mmio_write(uintptr_t reg, uint32_t data)
{
  *(uint32_t*)reg = data;
}
