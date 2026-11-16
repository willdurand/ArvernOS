/** @file */
#ifndef MMU_PAGING_H
#define MMU_PAGING_H

#include <stdint.h>

/// The default page size.
#define PAGE_SIZE 4096

/// Number of entries in a translation table. The 64-bit architectures
/// (x86_64, aarch64) use a 4 KiB granule with 512 entries per table.
#define PAGE_ENTRIES 512

/// A page table: 512 64-bit entries.
typedef struct page_table
{
  uint64_t entries[PAGE_ENTRIES];
} page_table_t;

typedef uint64_t page_number_t;

/**
 * @brief Initializes paging and enables the MMU.
 *
 * This is the architecture-independent entry point. The actual work is done by
 * the architecture-specific `arch_paging_init()` implementation.
 */
void paging_init();

/**
 * Returns the virtual start address of a page.
 *
 * @param page_number a page number
 * @returns a virtual address (page)
 */
uint64_t paging_page_to_address(page_number_t page_number);

/**
 * Calculates how many pages are needed for a range of addresses.
 *
 * @param start_address the starting virtual address
 * @param byte_size the amount of bytes
 * @return the amount of pages required for mapping from start_address until
 * start_address + byte_size
 */
uint32_t paging_page_amount_for_byte_size(uint64_t start_address,
                                          uint64_t byte_size);

/**
 * Returns the page number containing a virtual address.
 *
 * This is the architecture-independent entry point. The actual work is done by
 * the architecture-specific `arch_paging_page_containing_address()`
 * implementation.
 *
 * @param virtual_address a virtual address
 * @returns a page number
 */
page_number_t paging_page_containing_address(uint64_t virtual_address);

/**
 * Maps a page to a freshly allocated frame in the active address space.
 *
 * @param page_number a page number (not an address)
 * @param flags paging flags
 */
void paging_map(page_number_t page_number, uint64_t flags);

/**
 * Maps multiple pages in a row, allocating one frame per page.
 *
 * @param start_page_number the first page of the sequence
 * @param number_of_pages the amount of pages to map
 * @param flags the paging flags
 */
void paging_map_multiple(page_number_t start_page_number,
                         uint32_t number_of_pages,
                         uint64_t flags);

/**
 * Maps a virtual address to the equivalent frame (physical address) in the
 * active address space.
 *
 * @param address the address to map
 * @param flags paging flags
 */
void paging_identity_map(uint64_t address, uint64_t flags);

/**
 * Creates a new (empty) user address space that shares the kernel mappings.
 *
 * @return the physical address of the new top-level table, or `NULL` on failure
 */
void* paging_create_user_space(void);

/**
 * Clones the current address space, eagerly copying every user page.
 *
 * @return the physical address of the new top-level table, or `NULL` on failure
 */
void* paging_clone_user(void);

/**
 * Activates an address space.
 *
 * @param space an address space returned by `paging_create_user_space()` or
 * `paging_clone_user()`, or `NULL` for the kernel address space
 */
void paging_activate(void* space);

/**
 * Deactivates an address space, freeing its user pages and private page tables.
 *
 * @param space the address space to deactivate
 */
void paging_deactivate(void* space);

#endif
