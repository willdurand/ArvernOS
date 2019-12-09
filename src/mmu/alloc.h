#ifndef MMU_ALLOC_H
#define MMU_ALLOC_H

#define HEAP_START  0x100000
#define HEAP_SIZE   150 * 1024 // 150 KiB

/**
 * Initializes the heap memory allocator.
 */
void alloc_init();

#endif
