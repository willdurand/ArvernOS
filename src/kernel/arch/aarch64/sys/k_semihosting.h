#ifndef SYS_K_SEMIHOSTING_H
#define SYS_K_SEMIHOSTING_H

#include <stdint.h>

#define SYS_EXIT 0x18

void k_semihosting(uint64_t operation, uint64_t parameters);

#endif
