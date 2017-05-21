#include "mmap.h"
#include <mmu/paging.h>
#include <core/debug.h>

multiboot_tag_mmap_t *memory_area;
physical_address_t kernel_reserved_start;
physical_address_t kernel_reserved_end;
physical_address_t multiboot_reserved_start;
physical_address_t multiboot_reserved_end;
frame_t next_free_frame;

void mmap_init(
    multiboot_tag_mmap_t *mmap,
    const physical_address_t kernel_start, const physical_address_t kernel_end,
    const physical_address_t mb_start, const physical_address_t mb_end
) {
    memory_area = mmap;
    kernel_reserved_start = kernel_start;
    kernel_reserved_end = kernel_end;
    multiboot_reserved_start = mb_start;
    multiboot_reserved_end = mb_end;
    next_free_frame = 1;

    DEBUG(
        "Initialized MMAP with memory_area = 0x%x, multiboot_reserved_start = 0x%x, "
        "multiboot_reserved_end = 0x%x, next_free_frame = %d",
        memory_area,
        multiboot_reserved_start,
        multiboot_reserved_end,
        next_free_frame
    );
}

physical_address_t mmap_read(frame_t request, uint8_t mode)
{
    // If the user specifies an invalid mode, also skip the request
    if (mode != MMAP_GET_NUM && mode != MMAP_GET_ADDR) {
        return 0;
    }

    DEBUG("request = %d, mode = %d", request, mode);

    frame_t cur_num = 0;
    multiboot_mmap_entry_t *entry;
    for (
        entry = ((multiboot_tag_mmap_t *) memory_area)->entries;
        (uint8_t *) entry < (uint8_t *) memory_area + memory_area->size;
        entry = (multiboot_mmap_entry_t *) ((unsigned long) entry + ((multiboot_tag_mmap_t *) memory_area)->entry_size)
    ) {
        physical_address_t i;
        physical_address_t entry_end = entry->addr + entry->len;
        for (i = entry->addr; i + PAGE_SIZE < entry_end; i += PAGE_SIZE) {
            if (mode == MMAP_GET_NUM && request >= i && request <= i + PAGE_SIZE) {
                // If we're looking for a frame number from an address and we
                // found it return the frame number
                return cur_num + 1;
            }

            // If the requested chunk is in reserved space, skip it
            if (entry->type == MULTIBOOT_MEMORY_RESERVED) {
                if (mode == MMAP_GET_ADDR && cur_num == request) {
                    // The address of a chunk in reserved space was requested
                    // Increment the request until it is no longer reserved
                    request++;
                }
                // Skip to the next chunk until it's no longer reserved
                cur_num++;
                continue;
            } else if (mode == MMAP_GET_ADDR && cur_num == request) {
                // If we're looking for a frame starting address and we found
                // it return the starting address
                return i;
            }
            cur_num++;
        }
    }

    return 0;
}

frame_t mmap_allocate_frame()
{
    DEBUG("start (next_free_frame = %d)", next_free_frame);

    // Get the address for the next free frame
    physical_address_t current_addr = mmap_read(next_free_frame, MMAP_GET_ADDR);

    DEBUG("current_addr = 0x%x", current_addr);

    // Verify that the frame is not in the reserved areas. If it is, increment
    // the next free frame number and recursively call back.
    if (
        (current_addr >= multiboot_reserved_start && current_addr <= multiboot_reserved_end) ||
        (current_addr >= kernel_reserved_start && current_addr <= kernel_reserved_end)
    ) {
        next_free_frame++;

        return mmap_allocate_frame();
    }

    // TODO: add check for kernel reserved area

    // Call mmap_read again to get the frame number for our address
    frame_t current_frame_num = mmap_read(current_addr, MMAP_GET_NUM);

    // Update next_free_frame to the next unallocated frame number
    next_free_frame = current_frame_num + 1;

    DEBUG("return current_frame_num = %d (next_free_frame = %d)", current_frame_num, next_free_frame);

    // Finally, return the newly allocated frame num
    return current_frame_num;
}
