/** @file */
#ifndef MMU_ALLOC_H
#define MMU_ALLOC_H

#include <mmu/frame.h>
#include <sys/types.h>

/// The virtual address to the start of the heap memory.
#define HEAP_START 0x10000000
/// The size of the heap (10 MiB).
#define HEAP_SIZE (10 * 1024 * 1024)
/// The number of pages that we can allocate.
#define MAX_PAGES (HEAP_SIZE / PAGE_SIZE)

/**
 * Initializes the heap memory allocator.
 */
void alloc_init();

uint64_t alloc_get_used_count();

uint64_t alloc_get_max_count();

#endif
