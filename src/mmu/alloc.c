#include "alloc.h"
#include <kernel/panic.h>
#include <mmu/bitmap.h>
#include <mmu/debug.h>
#include <mmu/paging.h>
#include <mem.h>

bitmap_t allocated_pages[1];
uint64_t heap_end_page;
uint64_t heap_start_page;
uint64_t max_pages;

void alloc_init() {
    allocated_pages[0] = 0;
    heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
    heap_start_page = page_containing_address(HEAP_START);
    max_pages = heap_end_page - heap_start_page + 1;


    for (uint64_t i = heap_start_page; i <= heap_end_page; i++) {
        map(i, PAGING_FLAG_WRITABLE);
    }

    MMU_DEBUG(
        "heap_start_page=%u heap_end_page=%u max_pages=%u",
        heap_start_page, heap_end_page, max_pages
    );
}

int liballoc_lock() {
    // not implemented
    return 0;
}

int liballoc_unlock() {
    // not implemented
    return 0;
}

void* liballoc_alloc(int number_of_pages) {
    uint64_t first_free_page = 0;

    for (uint64_t i = 1; i <= max_pages; i++) {
        if (bitmap_get(allocated_pages, i) == false) {
            first_free_page = i;
            break;
        }
    }

    if (first_free_page == 0) {
        MMU_DEBUG("%s", "out of memory heap");
        return 0;
    }

    uint64_t addr = (HEAP_START + ((first_free_page - 1) * PAGE_SIZE));
    // TODO: I am actually not sure about this, but that should not hurt I
    // guess.
    void* ptr = memset((void*)addr, 0, (first_free_page - 1) * PAGE_SIZE);

    for (uint64_t i = 0; i < number_of_pages; i++) {
        bitmap_set(allocated_pages, first_free_page + i);
    }

    MMU_DEBUG("allocated %d pages at addr=%p", number_of_pages, addr);

    return ptr;
}

int liballoc_free(void* ptr, int number_of_pages) {
    uint64_t page = page_containing_address((uint64_t)ptr);

    for (uint64_t i = 0; i < number_of_pages; i++) {
        bitmap_clear(allocated_pages, page + i);
        unmap(page + i);
    }

    MMU_DEBUG("free'ed ptr=%p page=%u number_of_pages=%d", ptr, page, number_of_pages);

    return 0;
}
