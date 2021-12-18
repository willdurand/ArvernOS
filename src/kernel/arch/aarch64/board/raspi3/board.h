#ifndef BOARD_H
#define BOARD_H

#include <sys/types.h>

#define MMIO_BASE 0x3F000000
#define GPIO_BASE (MMIO_BASE + 0x200000)

// interrupts
#define IRQ_PENDING_1      (MMIO_BASE + 0x0000B204)
#define ENABLE_IRQS_1      (MMIO_BASE + 0x0000B210)
#define SYSTEM_TIMER_IRQ_1 (1 << 1)

#endif
