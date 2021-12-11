#ifndef SYS_K_SEMIHOSTING_H
#define SYS_K_SEMIHOSTING_H

#include <stdint.h>

#define SYS_EXIT_EXTENDED 0x20

void k_semihosting(uint32_t operation, uint32_t parameters);

#endif
