#ifndef MMU_PAGING_H
#define MMU_PAGING_H

#include <stdint.h>
#include <mmu/mmap.h>

#define PAGE_ENTRIES    512
#define P4_TABLE        0xfffffffffffff000

typedef uint64_t virtual_address_t;
typedef uint64_t page_t;

// cf.http://os.phil-opp.com/modifying-page-tables.html#page-table-entries
// cf. https://github.com/tmathmeyer/sos
typedef union page_entry {
    uint64_t packed;
    struct {
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
        uint64_t _addr_mask : 40;
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
} __attribute__((packed)) page_entry_t;

typedef page_entry_t* page_table_t;

void paging_init();
page_t page_containing_address(uint64_t addr);
uint64_t page_starting_address(page_t page);
physical_address_t translate(virtual_address_t addr);
frame_t translate_page(page_t page);
void map_page_to_frame(page_t page, frame_t frame, uint8_t flags);
void identity_map(frame_t frame, uint8_t flags);
void map(page_t page, uint8_t flags);
void unmap(page_t page);

#endif
