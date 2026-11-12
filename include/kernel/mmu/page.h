/** @file */
#ifndef MMU_PAGE_H
#define MMU_PAGE_H

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
 * Returns the virtual start address of a page.
 *
 * @param page_number a page number
 * @returns a virtual address (page)
 */
uint64_t page_start_address(page_number_t page_number);

/**
 * Calculates how many pages are needed for a range of addresses.
 *
 * @param start_address the starting virtual address
 * @param byte_size the amount of bytes
 * @return the amount of pages required for mapping from start_address until
 * start_address + byte_size
 */
uint32_t paging_amount_for_byte_size(uint64_t start_address,
                                     uint64_t byte_size);

#endif
