#include "mmap.h"
#include <mmu/paging.h>
#include <core/debug.h>

multiboot_tag_mmap_t *memory_area;
physical_address_t kernel_start;
physical_address_t kernel_end;
physical_address_t multiboot_start;
physical_address_t multiboot_end;
frame_t next_free_frame;

void mmap_init(multiboot_tag_mmap_t *mmap, reserved_areas_t reserved) {
    memory_area = mmap;
    kernel_start = reserved.kernel_start;
    kernel_end = reserved.kernel_end;
    multiboot_start = reserved.multiboot_start;
    multiboot_end = reserved.multiboot_end;
    next_free_frame = 1;

    DEBUG(
        "Initialized MMAP with memory_area = 0x%x, multiboot_start = 0x%x, "
        "multiboot_end = 0x%x, next_free_frame = %d",
        memory_area,
        multiboot_start,
        multiboot_end,
        next_free_frame
    );
}

physical_address_t mmap_read(frame_t request, uint8_t mode)
{
    frame_t cur_num = 0;

    for (
        multiboot_mmap_entry_t *entry = memory_area->entries;
        (uint8_t *) entry < (uint8_t *) memory_area + memory_area->size;
        entry = (multiboot_mmap_entry_t *) ((physical_address_t) entry + memory_area->entry_size)
    ) {
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        physical_address_t entry_end = entry->addr + entry->len;
        for (physical_address_t i = entry->addr; i + PAGE_SIZE < entry_end; i += PAGE_SIZE) {
            if ((i >= multiboot_start && i <= multiboot_end) || (i >= kernel_start && i <= kernel_end)) {
                continue;
            }

            if (mode == MMAP_GET_NUM && request >= i && request <= i + PAGE_SIZE) {
                return cur_num + 1;
            }

            if (mode == MMAP_GET_ADDR && cur_num == request && i != 0) {
                return i;
            }

            cur_num++;
        }
    }

    return 0;
}

frame_t mmap_allocate_frame()
{
    // Get the address for the next free frame
    physical_address_t addr = mmap_read(next_free_frame, MMAP_GET_ADDR);

    if (addr == 0) {
        return 0;
    }

    return next_free_frame++;
}

frame_t frame_containing_address(physical_address_t addr)
{
    return addr / PAGE_SIZE;
}

physical_address_t frame_starting_address(frame_t frame)
{
    return frame * PAGE_SIZE;
}

void mmap_deallocate_frame(frame_t frame)
{
    physical_address_t addr = frame_starting_address(frame);

    for (int i = 0; i < PAGE_SIZE; i++) {
        ((physical_address_t *)addr)[i] = 0;
    }
}
