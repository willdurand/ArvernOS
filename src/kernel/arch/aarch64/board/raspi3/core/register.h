#ifndef CORE_REGISTER_H
#define CORE_REGISTER_H

// The constants below come from [the raspberrypi-pi-os project][1], which is
// licensed under the MIT license: Copyright (c) 2018 Sergey Matyukevich.
//
// [1]: https://github.com/s-matyukevich/raspberry-pi-os/

// System Control Register
#define SCTLR_RESERVED         (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN (0 << 25)
#define SCTLR_I_CACHE_DISABLED (0 << 12)
#define SCTLR_D_CACHE_DISABLED (0 << 2)
#define SCTLR_MMU_DISABLED     (0 << 0)
#define SCTLR_VALUE_MMU_DISABLED                                               \
  (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED |          \
   SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// Saved Program Status Register
#define SPSR_MASK_ALL (7 << 6)
#define SPSR_EL1h     (5 << 0)
#define SPSR_VALUE    (SPSR_MASK_ALL | SPSR_EL1h)

#endif
