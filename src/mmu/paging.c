#include "paging.h"
#include <mmu/debug.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <mem.h>
#include <stdlib.h>

#define MMU_DEBUG_PAGE_ENTRY(message, e)  MMU_DEBUG("%s " \
                                          "page entry addr=%p present=%d " \
                                          "writable=%d no_execute=%d", \
                                          message, e.addr, e.present, \
                                          e.writable, e.no_execute)

void zero_table(page_table_t* table);
page_table_t* next_table_address(page_table_t* table, uint64_t index);
uint64_t translate(uint64_t virtual_address);
uint64_t pointed_frame(page_entry_t entry);
uint64_t p4_index(uint64_t page);
uint64_t p3_index(uint64_t page);
uint64_t p2_index(uint64_t page);
uint64_t p1_index(uint64_t page);
page_table_t* next_table_create(page_table_t* table, uint64_t index);
page_table_t* get_p4();
void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags);
uint64_t read_cr3();
void write_cr3(uint64_t value);

uint64_t (*allocate_frame)(void);
void (*deallocate_frame)(uint64_t);

void paging_init(multiboot_info_t* mbi) {
    UNUSED(*mbi);

    // Set allocator functions.
    allocate_frame = frame_allocate;
    deallocate_frame = frame_deallocate;

    // The rest of this function tests the paging feature.

    uint64_t addr1 = (uint64_t)42 * PAGE_ENTRIES * PAGE_ENTRIES * PAGE_SIZE;
    uint64_t addr2 = (uint64_t)43 * PAGE_ENTRIES * PAGE_ENTRIES * PAGE_SIZE;
    uint64_t addr3 = addr2 + PAGE_SIZE;
    uint64_t addr4 = addr3 + PAGE_SIZE;

    uint64_t page1 = page_containing_address(addr1);
    uint64_t page2 = page_containing_address(addr2);
    uint64_t page3 = page_containing_address(addr3);
    uint64_t page4 = page_containing_address(addr4);
    uint64_t frame1 = allocate_frame();
    uint64_t frame2 = allocate_frame();

    uint64_t* m1 = (uint64_t*)page_start_address(page1);
    uint64_t* m2 = (uint64_t*)page_start_address(page2);
    uint64_t* m3 = (uint64_t*)page_start_address(page3);
    uint64_t* m4 = (uint64_t*)page_start_address(page4);

    map_page_to_frame(page1, frame1, PAGING_FLAG_WRITABLE);
    map_page_to_frame(page2, frame1, PAGING_FLAG_WRITABLE);
    map_page_to_frame(page3, frame1, 0x0);
    map_page_to_frame(page4, frame2, 0x0);

    MMU_DEBUG("m1=%p (%x) m2=%p (%x) m3=%p (%x) and m4=%d", m1, *m1, m2, *m2, m3, *m3, m4);

    *m2 = 0xdeadbeef;
    m4 = 12;

    if (*m1 != *m3) {
        PANIC("%s", "paging test failed");
    }

    MMU_DEBUG("m1=%p (%x) m2=%p (%x) m3=%p (%x) and m4=%d", m1, *m1, m2, *m2, m3, *m3, m4);

    unmap(page1);
    unmap(page2);
    unmap(page3);
    unmap(page4);
}

void zero_table(page_table_t* table) {
    memset((void*)table, 0, sizeof(page_table_t));
}

page_table_t* next_table_address(page_table_t* table, uint64_t index) {
    if (table->entries[index].present != 1) {
        MMU_DEBUG("entry at index=%u in table=%p is not present, returning 0", index, table);
        return 0;
    }

    if (table->entries[index].huge_page == 1) {
        MMU_DEBUG("huge page detected for table=%p index=%u, returning 0", table, index);
        return 0;
    }

    page_table_t* next_table = (page_table_t*)(((uint64_t)table << 9) | (index << 12));

    MMU_DEBUG("addr=%p next table=%p", (uint64_t)table, next_table);

    return next_table;
}

// Translates a virtual address to the corresponding physical address.
uint64_t translate(uint64_t virtual_address) {
    uint64_t offset = virtual_address % PAGE_SIZE;
    uint64_t page = page_containing_address(virtual_address);
    uint64_t frame = translate_page(page);

    return frame * PAGE_SIZE + offset;
}

uint64_t page_containing_address(uint64_t virtual_address) {
    // So the address space is split into two halves: the higher half
    // containing addresses with sign extension and the lower half containing
    // addresses without. Everything in between is invalid.
    if (virtual_address < 0x0000800000000000 || virtual_address >= 0xffff800000000000) {
        return virtual_address / PAGE_SIZE;
    }

    PANIC("invalid address=%#x", virtual_address);
    return 0; // never reached but avoid a GCC warning.
}

uint64_t page_start_address(uint64_t page_number) {
    return page_number * PAGE_SIZE;
}

uint64_t translate_page(uint64_t page_number) {
    page_table_t* p4 = get_p4();

    page_table_t* p3 = next_table_address(p4, p4_index(page_number));

    if (p4->entries[p4_index(page_number)].huge_page) {
        uint64_t frame_number = pointed_frame(p3->entries[p3_index(page_number)]);
        MMU_DEBUG("1GB huge page=%u frame=%u", page_number, frame_number);

        if (frame_number % (PAGE_ENTRIES * PAGE_ENTRIES) == 0) {
            frame_number += p2_index(page_number) * PAGE_ENTRIES + p1_index(page_number);

            return frame_number;
        } else {
            PANIC("misaligned 1GB page=%u", page_number);
        }
    }

    if (p3 == 0) {
        MMU_DEBUG("did not find p3 (%p), returning 0", p3);
        return 0;
    }

    page_table_t* p2 = next_table_address(p3, p3_index(page_number));

    if (p3->entries[p3_index(page_number)].huge_page) {
        uint64_t frame_number = pointed_frame(p2->entries[p2_index(page_number)]);
        MMU_DEBUG("2MB huge page=%u frame=%u", page_number, frame_number);

        if (frame_number % PAGE_ENTRIES == 0) {
            frame_number += p1_index(page_number);

            return frame_number;
        } else {
            PANIC("misaligned 2MB page=%u", page_number);
        }
    }

    if (p2 == 0) {
        MMU_DEBUG("did not find p2 (%p), returning 0", p2);
        return 0;
    }

    page_table_t* p1 = next_table_address(p2, p2_index(page_number));

    if (p1 == 0) {
        MMU_DEBUG("did not find p1 (%p), returning 0", p1);
        return 0;
    }

    return pointed_frame(p1->entries[p1_index(page_number)]);
}

uint64_t p4_index(uint64_t page) {
    return (page >> 27) & 0777;
}

uint64_t p3_index(uint64_t page) {
    return (page >> 18) & 0777;
}

uint64_t p2_index(uint64_t page) {
    return (page >> 9) & 0777;
}

uint64_t p1_index(uint64_t page) {
    return (page >> 0) & 0777;
}

uint64_t pointed_frame(page_entry_t entry) {
    if (entry.present) {
        return frame_containing_address((uint64_t)entry.addr);
    }

    return 0;
}

void map_page_to_frame(uint64_t page, uint64_t frame, uint64_t flags) {
    MMU_DEBUG(
        "mapping page=%u to frame=%p (number=%d) with flags=%#x",
        page, frame, frame_containing_address(frame), flags
    );

    uint64_t frame_number = frame_containing_address(frame);

    if (translate_page(page) == frame_number) {
        MMU_DEBUG("page=%u already mapped to frame=%u", page, frame_number);
        return;
    }

    uint64_t p4_idx = p4_index(page);
    uint64_t p3_idx = p3_index(page);
    uint64_t p2_idx = p2_index(page);
    uint64_t p1_idx = p1_index(page);

    MMU_DEBUG("p1_idx=%u p2_idx=%u p3_idx=%u p4_idx=%u", p1_idx, p2_idx, p3_idx, p4_idx);

    MMU_DEBUG("p4=%p", get_p4());

    MMU_DEBUG("%s", "get or create p3");
    page_table_t* p3 = next_table_create(get_p4(), p4_idx);
    MMU_DEBUG("p3=%p", p3);

    MMU_DEBUG("%s", "get or create p2");
    page_table_t* p2 = next_table_create(p3, p3_idx);
    MMU_DEBUG("p2=%p", p2);

    MMU_DEBUG("%s", "get or create p1");
    page_table_t* p1 = next_table_create(p2, p2_idx);
    MMU_DEBUG("p1=%p", p1);

    page_entry_t entry = p1->entries[p1_idx];

    if (entry.addr != 0) {
        PANIC("%s", "entry should be unused");
    }

    paging_set_entry(&entry, frame, flags | PAGING_FLAG_PRESENT);

    p1->entries[p1_idx] = entry;

    MMU_DEBUG(
        "mapped page=%u to frame=%p (number=%d)...",
        page, frame, frame_containing_address(frame)
    );
    MMU_DEBUG_PAGE_ENTRY("...with", p1->entries[p1_idx]);

    MMU_DEBUG("entry addr=%p in p1[%d]", p1->entries[p1_idx].addr, p1_idx);
}

page_table_t* next_table_create(page_table_t* table, uint64_t index) {
    MMU_DEBUG("table=%p index=%d", table, index);

    bool was_created = false;

    page_entry_t entry = table->entries[index];

    if (entry.present) {
        MMU_DEBUG("page entry at index=%d is present", index);
    } else {
        MMU_DEBUG("page entry at index=%d is not present, creating entry", index);

        uint64_t frame = allocate_frame();

        if (frame == 0) {
            PANIC("%s", "frame is unexpectedly equal to 0");
        }

        paging_set_entry(&entry, frame, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

        table->entries[index] = entry;

        MMU_DEBUG_PAGE_ENTRY("created", table->entries[index]);

        was_created = true;
    }

    page_table_t* next_table = next_table_address(table, index);
    MMU_DEBUG("next table=%p", next_table);

    if (was_created) {
        zero_table(next_table);
        MMU_DEBUG("zero'ed table=%p", next_table);
    }

    return next_table;
}

void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags) {
    if ((addr & !0x000ffffffffff000) != 0) {
        PANIC("trying to set an invalid address: %p", addr);
    }

    entry->addr = addr;

    if (flags & PAGING_FLAG_PRESENT) {
        entry->present = 1;
    }

    if (flags & PAGING_FLAG_WRITABLE) {
        entry->writable = 1;
    }

    if (flags & PAGING_FLAG_NO_EXECUTE) {
        entry->no_execute = 1;
    }

    MMU_DEBUG_PAGE_ENTRY("set", (*entry));
}

page_table_t* get_p4() {
    return (page_table_t*)P4_TABLE;
}

void unmap(uint64_t page_number) {
    uint64_t addr = page_start_address(page_number);

    if (translate(addr) == 0) {
        PANIC("cannot unmap page=%u because it is not mapped", page_number);
    }

    page_table_t* p4 = get_p4();
    page_table_t* p3 = next_table_address(p4, p4_index(page_number));
    page_table_t* p2 = next_table_address(p3, p3_index(page_number));
    page_table_t* p1 = next_table_address(p2, p2_index(page_number));

    page_entry_t entry = p1->entries[p1_index(page_number)];
    uint64_t frame_number = pointed_frame(entry);

    p1->entries[p1_index(page_number)].addr = 0;
    MMU_DEBUG_PAGE_ENTRY("cleared", p1->entries[p1_index(page_number)]);

    /// @todo free p(1,2,3) table if empty

    deallocate_frame(frame_number);

    // flush the translation lookaside buffer
    // cf. http://os.phil-opp.com/modifying-page-tables.html#unmap
    __asm__("invlpg (%0)" : /* no output */ : "r"(addr) : "memory");

    MMU_DEBUG("unapped page=%u addr=%p", page_number, addr);
}

uint64_t read_cr3() {
    uint64_t value;

    __asm__("mov %%cr3, %0" : "=r"(value) : /* no input */);

    return value;
}

void write_cr3(uint64_t value) {
    __asm__("mov %0, %%cr3" : /* no output */ : "r"(value));
}

void map(uint64_t page_number, uint64_t flags) {
    uint64_t frame = allocate_frame();

    if (frame == 0) {
        PANIC("out of memory");
    }

    map_page_to_frame(page_number, frame, flags);
}
