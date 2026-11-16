/** @file */
#ifndef ARCH_PAGING_H
#define ARCH_PAGING_H

#include <mmu/frame.h>
#include <mmu/paging.h>
#include <stdint.h>
#include <sys/types.h>

/// End of the user address space.
#define USER_SPACE_END 0x80000000

/// Specifies whether the mapped page is loaded in memory.
#define PAGING_FLAG_PRESENT (1 << 0)
/// Controls whether writes to the mapped frames are allowed.
#define PAGING_FLAG_WRITABLE (1 << 1)
/// Controls whether accesses from userspace (i.e. ring 3) are permitted.
#define PAGING_FLAG_USER_ACCESSIBLE (1 << 2)
/// Tells whether the mapped page is a huge page.
#define PAGING_FLAG_HUGE_PAGE (1 << 7)
/// Forbids executing code on this page.
#define PAGING_FLAG_NO_EXECUTE ((uint64_t)1 << 63)

/**
 * Translates a page number to a frame number.
 *
 * @param page_number a page number (not an address)
 * @returns a frame number (optional)
 */
opt_uint64_t translate_page(page_number_t page_number);

/**
 * Translates a virtual address to the corresponding physical address.
 *
 * @param virtual_address a virtual address
 * @returns a physical address (optional)
 */
opt_uint64_t translate(uint64_t virtual_address);

/**
 * Unmaps (free) a page.
 *
 * @param page_number a page number (not an address)
 */
void unmap(page_number_t page_number);

/**
 * Unmaps multiple consecutive pages.
 *
 * @param start_page_number the first page of the sequence
 * @param number_of_pages the amount of pages to unmap
 */
void unmap_multiple(page_number_t start_page_number, uint32_t number_of_pages);

// The functions below are exposed for testing purposes.

void _set_p4(page_table_t* table);
uint64_t _p4_index(page_number_t page_number);
uint64_t _p3_index(page_number_t page_number);
uint64_t _p2_index(page_number_t page_number);
uint64_t _p1_index(page_number_t page_number);

#ifdef TEST_ENV
opt_uint64_t test_frame_allocate();
void test_frame_deallocate(frame_number_t frame_number);
page_table_t* test_next_table();
void test_frame_mark_as_used(uint64_t address);
#endif

#endif
