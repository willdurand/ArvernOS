#include "alloc.h"
#include <kernel/panic.h>
#include <mmu/debug.h>
#include <mmu/paging.h>

uint64_t available_pages;
uint64_t heap_end_page;
uint64_t heap_start_page;
uint64_t next_free_page;

void alloc_init() {
    available_pages = HEAP_SIZE / PAGE_SIZE;
    heap_start_page = page_containing_address(HEAP_START);
    heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);

    next_free_page = 0;

    MMU_DEBUG(
        "heap_start_page=%u heap_end_page=%u available_pages=%u",
        heap_start_page, heap_end_page, available_pages
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
    if ((uint32_t)number_of_pages > available_pages) {
        PANIC("cannot allocate %d pages, out of heap memory", number_of_pages);
    }

    void* ptr = HEAP_START + (next_free_page * PAGE_SIZE);
    MMU_DEBUG("allocating %d pages", number_of_pages);

    for (int i = 0; i < number_of_pages; i++) {
        map(heap_start_page + next_free_page, PAGING_FLAG_WRITABLE);
        next_free_page++;
    }

    available_pages -= number_of_pages;

    MMU_DEBUG(
        "allocated %d pages at addr=%p (available pages=%d)",
        number_of_pages, ptr, available_pages
    );

    return ptr;
}

int liballoc_free(void* ptr, int number_of_pages) {
    MMU_DEBUG("free'ed ptr=%p number_of_pages=%d", ptr, number_of_pages);

    /// @todo actually free pages

    return 0;
}
