/**
 * @file
 * @see http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html
 *
 * ARM tags are used to pass information from the bootloader to the kernel. It
 * is kind of legacy now because Linux uses Device Trees, but it is easier to
 * implement in a project like ArvernOS.
 */
#ifndef CORE_ARM_ATAG_H
#define CORE_ARM_ATAG_H

#include <stdint.h>

#define ATAG_NONE      0
#define ATAG_CORE      0x54410001
#define ATAG_MEM       0x54410002
#define ATAG_VIDEOTEXT 0x54410003
#define ATAG_RAMDISK   0x54410004
#define ATAG_INITRD2   0x54420005
#define ATAG_SERIAL    0x54410006
#define ATAG_REVISION  0x54410007
#define ATAG_VIDEOLFB  0x54410008
#define ATAG_CMDLINE   0x54410009

typedef struct atag_header
{
  uint32_t size;
  uint32_t tag;
} atag_header_t;

typedef struct atag_mem
{
  atag_header_t header;
  uint32_t size;
  uint32_t start;
} atag_mem_t;

typedef struct atag_cmdline
{
  atag_header_t header;
  char cmdline[];
} atag_cmdline_t;

typedef struct atag_initrd2
{
  atag_header_t header;
  uint32_t start;
  uint32_t size;
} atag_initrd2_t;

/**
 * Initializes the ATAGs module, which essentially parses the ATAGs and store
 * information that can be queried later.
 *
 * @param ptr a pointer to the first ATAG header
 */
void atag_init(atag_header_t* ptr);

/**
 * Returns the command line found in the ATAGs.
 *
 * @return a string containing the command line found in the ATAGS or `NULL`
 */
const char* atag_get_cmdline();

/**
 * Returns the initrd2 entry at a specific index found in the ATAGs.
 *
 * @param index the index of the initrd2 entry to retrieve
 * @return the initrd2 entry or `NULL` if not found (or `index` is invalid)
 */
atag_initrd2_t* atag_get_initrd2_at(uint8_t index);

#endif
