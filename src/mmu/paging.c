#include "paging.h"
#include <mmu/mmap.h>
#include <core/debug.h>

uint64_t p4_index(frame_t frame);
uint64_t p3_index(frame_t frame);
uint64_t p2_index(frame_t frame);
uint64_t p1_index(frame_t frame);
frame_t pointed_frame(page_entry_t entry);
void set_addr_mask(page_entry_t *entry, uint64_t addr);
page_table_t next_table_create(page_table_t in, uint64_t index);

void paging_init()
{
}

page_t page_containing_address(uint64_t addr)
{
    return addr / PAGE_SIZE;
}

frame_t pointed_frame(page_entry_t entry)
{
    if (entry.present) {
        uint64_t e = entry.packed;
        e &= 0x000ffffffffff000;

        return frame_containing_address(e);
    }

    return 0;
}

uint64_t starting_address(uint64_t page_frame)
{
    return page_frame * PAGE_SIZE;
}

page_table_t next_table_address(page_table_t table, uint32_t index)
{
    if (table[index].present && !table[index].huge_page) {
        uint64_t table_address = (uint64_t) table;

        return (page_table_t) ((table_address << 9) | (index << 12));
    }

    return 0;
}

physical_address_t translate(virtual_address_t addr)
{
    page_t virt = page_containing_address(addr);
    uint64_t offset = virt % PAGE_SIZE;

    return (physical_address_t) (starting_address((uint64_t) translate_page(virt)) + offset);
}

frame_t translate_page(page_t page)
{
    page_table_t p3 = next_table_address((page_table_t)P4_TABLE, p4_index(page));
    if (!p3) {
        return 0;
    }

    // TODO: huge page

    page_table_t p2 = next_table_address(p3, p3_index(page));
    if (!p2) {
        return 0;
    }

    page_table_t p1 = next_table_address(p2, p2_index(page));
    if (!p1) {
        return 0;
    }

    page_entry_t entry = p1[p1_index(page)];

    return pointed_frame(entry);
}

uint64_t p4_index(frame_t frame)
{
    return (frame >> 27) & 0777;
}

uint64_t p3_index(frame_t frame)
{
    return (frame >> 18) & 0777;
}

uint64_t p2_index(frame_t frame)
{
    return (frame >> 9) & 0777;
}

uint64_t p1_index(frame_t frame)
{
    return (frame >> 0) & 0777;
}

void map_page_to_frame(page_t page, frame_t frame, uint8_t flags)
{
    page_table_t p3 = next_table_create((page_table_t)P4_TABLE, p4_index(page));

    DEBUG("p3 = 0x%X", p3);

    page_table_t p2 = next_table_create((page_table_t)p3, p3_index(page));

    DEBUG("p2 = 0x%X", p2);

    page_table_t p1 = next_table_create((page_table_t)p2, p2_index(page));

    set_addr_mask(&p1[p1_index(page)], frame);
    p1[p1_index(page)].packed = flags;
    p1[p1_index(page)].present = 1;
}

void set_addr_mask(page_entry_t *entry, uint64_t addr)
{
    uint64_t mask = 0xfff0000000000fff;
    entry->packed &= mask;
    entry->packed |= (addr * PAGE_SIZE);
}

page_table_t next_table_create(page_table_t table, uint64_t index)
{
    page_entry_t location = table[index];
    page_table_t res;

    DEBUG("index = %L - start", index);

    if (!location.present) {
        frame_t frame = mmap_allocate_frame();
        physical_address_t addr = mmap_read(frame, MMAP_GET_ADDR);

        DEBUG("index = %L, frame = %d, addr = 0x%X", index, frame, addr);

        set_addr_mask(&table[index], addr);
        table[index].present = 1;
        table[index].writable = 1;
    }

    res = next_table_address(table, index);

    return res;
}
