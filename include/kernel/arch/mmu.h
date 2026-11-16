/** @file */
#ifndef ARCH_MMU_H
#define ARCH_MMU_H

#include <mmu/paging.h>
#include <sys/types.h>

/**
 * @brief Initializes paging and enables the MMU.
 *
 * Architecture-specific implementation called by the shared `paging_init()`.
 */
void arch_paging_init();

/**
 * Maps a page to a physical frame in the active address space.
 *
 * @param page_number a page number (not an address)
 * @param frame a physical address (frame)
 * @param flags paging flags
 */
void arch_paging_map_page(page_number_t page_number,
                          uint64_t frame,
                          uint64_t flags);

/**
 * Allocates a page-aligned, zeroed frame.
 *
 * @returns the physical address of the frame (optional)
 */
opt_uint64_t arch_paging_allocate_frame(void);

/**
 * Creates a new (empty) user address space that shares the kernel mappings.
 *
 * @return the physical address of the new top-level table, or `NULL` on failure
 */
void* arch_paging_create_user_space(void);

/**
 * Clones the current address space, eagerly copying every user page.
 *
 * @return the physical address of the new top-level table, or `NULL` on failure
 */
void* arch_paging_clone_user(void);

/**
 * Activates an address space.
 *
 * @param space the address space to activate, or `NULL` for the kernel address
 * space
 */
void arch_paging_activate(void* space);

/**
 * Deactivates an address space, freeing its user pages and private page tables.
 *
 * @param space the address space to deactivate
 */
void arch_paging_deactivate(void* space);

/**
 * Returns the page number containing a virtual address.
 *
 * This is the architecture-specific address-to-page translation. The
 * architecture may reject addresses that the hardware cannot translate.
 *
 * @param virtual_address a virtual address
 * @returns a page number
 */
page_number_t arch_paging_page_containing_address(uint64_t virtual_address);

#endif
