/** @file */
#ifndef MMU_FRAME_H
#define MMU_FRAME_H

#include <core/multiboot.h>
#include <mmu/bitmap.h>
#include <sys/types.h>

/// The default page size.
#define PAGE_SIZE 4096

typedef uint64_t frame_number_t;

/**
 * Initializes the frame allocator (physical memory management).
 */
void frame_init();

/**
 * Allocates a frame and returns its physical address.
 *
 * @returns a physical address (frame) (optional)
 */
opt_uint64_t frame_allocate();

/**
 * Deallocates (free) a frame.
 *
 * @param frame_number a frame number (not an address)
 */
void frame_deallocate(frame_number_t frame_number);

/**
 * Returns the frame number given a physical address.
 *
 * @param physical_address a physical address (frame)
 * @returns a frame number
 */
frame_number_t frame_containing_address(uint64_t physical_address);

/**
 * Returns the physical start address of a frame.
 *
 * @param frame_number a frame number (not an address)
 * @returns a physical address (frame)
 */
uint64_t frame_start_address(frame_number_t frame_number);

/**
 * Returns the number of frames currently used.
 *
 * @return the number of frames currently used
 */
uint64_t frame_get_used_count();

/**
 * Returns the number of frames that the system can possibly allocate.
 *
 * @return the maximum number of frames
 */
uint64_t frame_get_max_count();

// The declarations below are for testing purposes.
void _frame_init(reserved_areas_t* reserved_areas, multiboot_tag_mmap_t* _mmap);
void _frame_set_bitmap(bitmap_t* bitmap);

#endif
