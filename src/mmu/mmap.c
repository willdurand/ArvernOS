#include "mmap.h"
#include <core/debug.h>
#include <kernel/panic.h>

multiboot_tag_mmap_t* memory_area;
uint64_t kernel_start;
uint64_t kernel_end;
uint64_t multiboot_start;
uint64_t multiboot_end;
uint64_t next_free_frame;

void mmap_init(multiboot_info_t* mbi) {
    reserved_areas_t reserved = read_multiboot_info(mbi);
    multiboot_tag_mmap_t* mmap = find_multiboot_tag(mbi->tags, MULTIBOOT_TAG_TYPE_MMAP);

    DEBUG("multiboot_start = %p, multiboot_end = %p", reserved.multiboot_start, reserved.multiboot_end);
    DEBUG("kernel_start    = %p, kernel_end    = %p", reserved.kernel_start, reserved.kernel_end);

    memory_area = mmap;
    kernel_start = reserved.kernel_start;
    kernel_end = reserved.kernel_end;
    multiboot_start = reserved.multiboot_start;
    multiboot_end = reserved.multiboot_end;
    next_free_frame = 1;

    DEBUG(
        "Initialized MMAP with memory_area = %p, multiboot_start = %p, "
        "multiboot_end = %p, next_free_frame = %u",
        memory_area,
        multiboot_start,
        multiboot_end,
        next_free_frame
    );
}

uint64_t mmap_read(uint64_t request, uint8_t mode) {
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

        for (uint64_t i = entry->addr; i + PAGE_SIZE < entry_end; i += PAGE_SIZE) {
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

uint64_t mmap_allocate_frame() {
    // Get the address for the next free frame
    uint64_t addr = mmap_read(next_free_frame, MMAP_GET_ADDR);

    if (addr == 0) {
        PANIC("failed to allocate a new frame, addr=%p", addr);
    }

    DEBUG("allocated new frame with addr=%p", addr);

    return next_free_frame++;
}

void mmap_deallocate_frame(uint64_t addr) {
    uint64_t frame = frame_starting_address(addr);

    for (int i = 0; i < PAGE_SIZE; i++) {
        ((uint64_t*)frame)[i] = 0;
    }
}

uint64_t frame_containing_address(uint64_t addr) {
    return addr / PAGE_SIZE;
}

uint64_t frame_starting_address(uint64_t addr) {
    return addr * PAGE_SIZE;
}
