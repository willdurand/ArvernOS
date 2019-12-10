#include "frame.h"
#include <mmu/debug.h>
#include <kernel/panic.h>

uint64_t read_mmap(uint64_t request);

multiboot_tag_mmap_t* memory_area;
uint64_t kernel_start;
uint64_t kernel_end;
uint64_t multiboot_start;
uint64_t multiboot_end;
uint64_t next_free_frame;

void frame_init(multiboot_info_t* mbi) {
    reserved_areas_t reserved = read_multiboot_info(mbi);
    multiboot_tag_mmap_t* mmap = find_multiboot_tag(mbi->tags, MULTIBOOT_TAG_TYPE_MMAP);

    MMU_DEBUG("multiboot_start = %p, multiboot_end = %p", reserved.multiboot_start,
              reserved.multiboot_end);
    MMU_DEBUG("kernel_start    = %p, kernel_end    = %p", reserved.kernel_start, reserved.kernel_end);

    memory_area = mmap;
    kernel_start = reserved.kernel_start;
    kernel_end = reserved.kernel_end;
    multiboot_start = reserved.multiboot_start;
    multiboot_end = reserved.multiboot_end;
    next_free_frame = 1;

    MMU_DEBUG(
        "Initialized frame allocator with multiboot_start = %p multiboot_end ="
        "%p kernel_start=%p kernel_end=%p next_free_frame = %u",
        multiboot_start,
        multiboot_end,
        kernel_start,
        kernel_end,
        next_free_frame
    );
}

uint64_t read_mmap(uint64_t request) {
    uint64_t cur_num = 0;

    for (
        multiboot_mmap_entry_t* entry = memory_area->entries;
        (uint8_t*) entry < (uint8_t*) memory_area + memory_area->size;
        entry = (multiboot_mmap_entry_t*)((uint64_t) entry + memory_area->entry_size)
    ) {
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        uint64_t entry_end = entry->addr + entry->len;

        for (uint64_t addr = entry->addr; addr + PAGE_SIZE < entry_end; addr += PAGE_SIZE) {
            if ((addr >= multiboot_start && addr <= multiboot_end) || (addr >= kernel_start &&
                    addr <= kernel_end)) {
                continue;
            }

            if (cur_num == request && addr != 0) {
                return addr;
            }

            cur_num++;
        }
    }

    return 0;
}

/**
 * This is a sort of "WaterMark allocator" because we can only create new
 * frames and we do not retain the created ones in order to reuse them after
 * they are deallocated.
 *
 * @todo create a map to retain allocated frames and update this function to
 * reuse deallocated frames. Maybe we could have a fixed-length map to start.
 * That would still be not ideal but that would be better than nothing.
 */
uint64_t frame_allocate() {
    // Get the address for the next free frame
    uint64_t addr = read_mmap(next_free_frame);

    if (addr == 0) {
        PANIC("failed to allocate a new frame, next_free_frame=%u", next_free_frame);
    }

    MMU_DEBUG("allocated new frame with addr=%p", addr);
    next_free_frame++;

    return addr;
}

void frame_deallocate(uint64_t frame_number) {
    uint64_t addr = frame_start_address(frame_number);

    /// @todo actually free this frame

    MMU_DEBUG("deallocated frame=%u addr=%p", frame_number, addr);
}

uint64_t frame_containing_address(uint64_t physical_address) {
    return physical_address / PAGE_SIZE;
}

uint64_t frame_start_address(uint64_t frame_number) {
    return frame_number * PAGE_SIZE;
}
