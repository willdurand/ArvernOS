#ifndef CORE_MMIO_H
#define CORE_MMIO_H

#include <sys/types.h>

#define MMIO_BASE 0x3F000000

#define GPCLR0    (MMIO_BASE + 0x00200028)
#define GPFSEL0   (MMIO_BASE + 0x00200000)
#define GPFSEL1   (MMIO_BASE + 0x00200004)
#define GPPUD     (MMIO_BASE + 0x00200094)
#define GPPUDCLK0 (MMIO_BASE + 0x00200098)
#define GPPUDCLK1 (MMIO_BASE + 0x0020009C)
#define GPPUPPDN0 (MMIO_BASE + 0x002000E4)
#define GPSET0    (MMIO_BASE + 0x0020001C)
#define GPSET1    (MMIO_BASE + 0x00200020)

// mini-uart
#define AUX_ENABLE     (MMIO_BASE + 0x00215004)
#define AUX_MU_IO      (MMIO_BASE + 0x00215040)
#define AUX_MU_IER     (MMIO_BASE + 0x00215044)
#define AUX_MU_IIR     (MMIO_BASE + 0x00215048)
#define AUX_MU_LCR     (MMIO_BASE + 0x0021504C)
#define AUX_MU_MCR     (MMIO_BASE + 0x00215050)
#define AUX_MU_LSR     (MMIO_BASE + 0x00215054)
#define AUX_MU_MSR     (MMIO_BASE + 0x00215058)
#define AUX_MU_SCRATCH (MMIO_BASE + 0x0021505C)
#define AUX_MU_CNTL    (MMIO_BASE + 0x00215060)
#define AUX_MU_STAT    (MMIO_BASE + 0x00215064)
#define AUX_MU_BAUD    (MMIO_BASE + 0x00215068)

void mmio_write(uintptr_t reg, uint32_t data);

uint32_t mmio_read(uintptr_t reg);

#endif
