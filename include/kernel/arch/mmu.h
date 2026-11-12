/** @file */
#ifndef ARCH_MMU_H
#define ARCH_MMU_H

#include <mmu/page.h>

/**
 * Returns the page number containing a virtual address.
 *
 * This is the architecture-specific address-to-page translation. The
 * architecture may reject addresses that the hardware cannot translate.
 *
 * @param virtual_address a virtual address
 * @returns a page number
 */
page_number_t arch_page_containing_address(uint64_t virtual_address);

#endif
