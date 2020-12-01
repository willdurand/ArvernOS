/** @file */
#ifndef MMU_PAGING_H
#define MMU_PAGING_H

#include <core/multiboot.h>
#include <mmu/frame.h>
#include <stdint.h>
#include <sys/types.h>

#define PAGE_ENTRIES 512
#define P4_TABLE     0xfffffffffffff000

/// Specifies whether the mapped page is loaded in memory.
#define PAGING_FLAG_PRESENT 0x01
/// Controls whether writes to the mapped frames are allowed.
#define PAGING_FLAG_WRITABLE 0x02
/// Controls whether accesses from userspace (i.e. ring 3) are permitted.
#define PAGING_FLAG_USER_ACCESSIBLE ((uint64_t)1) << 2
/// Forbids executing code on this page.
#define PAGING_FLAG_NO_EXECUTE ((uint64_t)1) << 63

// cf.http://os.phil-opp.com/modifying-page-tables.html#page-table-entries
// cf. https://github.com/tmathmeyer/sos
typedef struct page_entry
{
  union
  {
    uint64_t packed;
    struct
    {
      uint8_t present : 1;
      uint8_t writable : 1;
      uint8_t user_accessible : 1;
      uint8_t write_thru_cache : 1;
      uint8_t disable_cache : 1;
      uint8_t accessed : 1;
      uint8_t dirty : 1;
      uint8_t huge_page : 1;
      uint8_t global : 1;
      uint8_t OS_1 : 1;
      uint8_t OS_2 : 1;
      uint8_t OS_3 : 1;
      uint64_t _addr : 40;
      uint8_t OS_4 : 1;
      uint8_t OS_5 : 1;
      uint8_t OS_6 : 1;
      uint8_t OS_7 : 1;
      uint8_t OS_8 : 1;
      uint8_t OS_9 : 1;
      uint8_t OS_A : 1;
      uint8_t OS_B : 1;
      uint8_t OS_C : 1;
      uint8_t OS_D : 1;
      uint8_t OS_E : 1;
      uint8_t no_execute : 1;
    };
  };
} __attribute__((packed)) page_entry_t;

typedef struct page_table
{
  page_entry_t entries[PAGE_ENTRIES];
} page_table_t;

typedef uint64_t page_number_t;

/**
 * @brief Initializes paging to separate virtual and physical memory.
 *
 * The bootloader (assembly code) already set up a 4-level paging hierarchy
 * that maps every page of our kernel to a physical frame. It does this because
 * paging is mandatory in 64-bit mode on x86_64.
 *
 * This means that every memory address that we used in our kernel was a
 * virtual address. Accessing the VGA buffer at address `0xb8000` only worked
 * because the bootloader identity mapped that memory page, which means that it
 * mapped the virtual page `0xb8000` to the physical frame `0xb8000`.
 *
 * In addition, this improves safety since illegal memory accesses cause page
 * fault exceptions instead of modifying arbitrary physical memory. That being
 * said, identity mapping clutters the virtual address space and makes it more
 * difficult to find continuous memory regions of larger sizes (like 1000 KiB).
 *
 * We remap the kernel and other useful sections/addresses in this function,
 * then we enable write protection, the NXE bit and we switch to our own table.
 * This allows us to make everything safer. Memory is still identity mapped,
 * which isn't great but it works for now.
 */
void paging_init(multiboot_info_t* mbi);

/**
 * Returns a page number given a virtual address.
 *
 * @param virtual_address a virtual address (page)
 * @returns a page number
 */
page_number_t page_containing_address(uint64_t virtual_address);

/**
 * Returns the virtual start address of a page.
 *
 * @param page_number a page number
 * @returns a virtual address (page)
 */
uint64_t page_start_address(page_number_t page_number);

/**
 * Translates a page number to a frame number.
 *
 * @param page_number a page number (not an address)
 * @returns a frame number (optional)
 */
opt_uint64_t translate_page(page_number_t page_number);

/**
 * Convenient function to map a pager (number) without providing a frame. The
 * frame is automatically allocated.
 *
 * @param page_number a page number (not an address)
 * @param flags paging flags
 */
void map(page_number_t page_number, uint64_t flags);

/**
 * Maps multiple pages in a row if possible.
 *
 * @param start_page_number the first page of the sequence
 * @param number_of_pages the amount of pages to map
 * @param flags the paging flags
 */
void map_multiple(page_number_t start_page_number,
                  uint32_t number_of_pages,
                  uint64_t flags);

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

/**
 * Maps a physical address directly
 * 
 * @param physical_address the physical address to map
 * @param flags the paging flags
 */
void identity_map(uint64_t physical_address, uint64_t flags);


// The functions below are exposed for testing purposes.

void _set_p4(page_table_t* table);
uint64_t _p3_index(page_number_t page_number);
uint64_t _p2_index(page_number_t page_number);
uint64_t _p1_index(page_number_t page_number);

#ifdef TEST_ENV
opt_uint64_t test_frame_allocate();
void test_frame_deallocate(frame_number_t frame_number);
page_table_t* test_next_table();
#endif

#endif
