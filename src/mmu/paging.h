#ifndef MMU_PAGING_H
#define MMU_PAGING_H

#include <core/boot.h>
#include <mmu/mmap.h>
#include <stdint.h>

#define PAGE_ENTRIES  512
#define P4_TABLE      0xfffffffffffff000

#define PAGING_FLAG_PRESENT     0x01
#define PAGING_FLAG_WRITABLE    0x02
#define PAGING_FLAG_NO_EXECUTE  ((uint64_t)1) << 63

// cf.http://os.phil-opp.com/modifying-page-tables.html#page-table-entries
// cf. https://github.com/tmathmeyer/sos
typedef struct page_entry {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user_accessable : 1;
    uint8_t write_thru_cache : 1;
    uint8_t disable_cache : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t huge_page : 1;
    uint8_t global : 1;
    uint8_t OS_1 : 1;
    uint8_t OS_2 : 1;
    uint8_t OS_3 : 1;
    uint64_t addr : 40;
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
}  __attribute__((packed)) page_entry_t;

typedef struct page_table {
    page_entry_t entries[PAGE_ENTRIES];
} page_table_t;

void paging_init(multiboot_info_t* mbi);
// Returns a page number.
uint64_t page_containing_address(uint64_t virtual_address);
// Returns a virtual address.
uint64_t page_start_address(uint64_t page);
// Returns a frame number.
uint64_t translate_page(uint64_t page);
void map_page_to_frame(uint64_t page, uint64_t frame, uint64_t flags);
void unmap(uint64_t page);

#endif
