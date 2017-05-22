#ifndef MMU_PAGING_H
#define MMU_PAGING_H

#include <stdint.h>
#include <mmu/mmap.h>

#define PAGE_SIZE       4096
#define PAGE_ENTRIES    512
#define P4_TABLE        0xfffffffffffff000

typedef uint64_t virtual_address_t;
typedef uint64_t page_t;

typedef union page_entry {
    uint64_t packed;
    struct {
        uint8_t present: 1,
                writable: 1,
                user_accessible: 1,
                write_through : 1,
                no_cache : 1,
                accessed : 1,
                dirty : 1,
                huge_page : 1;
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
