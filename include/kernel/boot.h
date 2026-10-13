/**
 * @file
 *
 * Boot information gathered by the architecture-specific boot code.
 */
#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

/// Facts about the current boot, as discovered by the arch-specific `kmain()`.
typedef struct boot_info
{
  uintptr_t initrd_addr; ///< Address of the initial ramdisk, or 0 if none
  uintptr_t initrd_size; ///< Size in bytes of the initial ramdisk, or 0
  const char* cmdline;   ///< Kernel command line, or `NULL` if none
} boot_info_t;

/**
 * Records the boot information.
 *
 * Must be called by the architecture-specific `kmain()` before it hands
 * control over to `kmain_start()`.
 *
 * @param info the boot information to set
 */
void boot_info_set(const boot_info_t* info);

/**
 * Returns the boot information collected so far.
 *
 * @return the current boot information
 */
const boot_info_t* boot_info_get();

#endif
