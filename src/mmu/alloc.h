#ifndef MMU_ALLOC_H
#define MMU_ALLOC_H

#define HEAP_START  0x40000000
#define HEAP_SIZE   200 * 1024 // 200 KiB

/**
 * Initializes the heap memory allocator.
 */
void alloc_init();

#endif
