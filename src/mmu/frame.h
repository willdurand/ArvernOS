/** @file */
#ifndef MMU_FRAME_H
#define MMU_FRAME_H

#include <core/boot.h>

/// The default page size.
#define PAGE_SIZE 4096

/// The number of frames that we can allocate (we consider 256MB for now).
#define MAX_FRAMES (256 * 1000000) / PAGE_SIZE

/**
 * Initializes the frame allocator (physical memory management).
 */
void frame_init(multiboot_info_t* mbi);

/**
 * Allocates a frame and returns its physical address.
 *
 * @returns a physical address (frame)
 */
uint64_t frame_allocate();

/**
 * Deallocates (free) a frame.
 *
 * @param frame_number a frame number (not an address)
 */
void frame_deallocate(uint64_t frame_number);

/**
 * Returns the frame number given a physical address.
 *
 * @param physical_address a physical address (frame)
 * @returns a frame number
 */
uint64_t frame_containing_address(uint64_t physical_address);

/**
 * Returns the physical start address of a frame.
 *
 * @param frame_number a frame number (not an address)
 * @returns a physical address (frame)
 */
uint64_t frame_start_address(uint64_t frame_number);

#endif
