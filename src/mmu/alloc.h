/** @file */
#ifndef MMU_ALLOC_H
#define MMU_ALLOC_H

/// The virtual address to the start of the heap memory.
#define HEAP_START 0x40000000
/// The size of the heap.
#define HEAP_SIZE 200 * 1024 // 200 KiB

/**
 * Initializes the heap memory allocator.
 */
void alloc_init();

#endif
