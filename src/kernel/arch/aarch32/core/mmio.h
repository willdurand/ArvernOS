#ifndef CORE_MMIO_H
#define CORE_MMIO_H

#include <stdint.h>

uint32_t mmio_read(uintptr_t reg);

void mmio_write(uintptr_t reg, uint32_t data);

#endif
