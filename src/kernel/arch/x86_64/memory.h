/**
 * @file
 *
 * Physical memory handoff between the board's bootloader and the MMU.
 *
 * A board learns the physical memory layout from its bootloader and normalizes
 * it here, so that the architecture's frame allocator and paging code stay
 * board-agnostic.
 */
#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <stdint.h>

/// Usable RAM.
#define MEMORY_AVAILABLE 1
/// Memory reserved.
#define MEMORY_RESERVED 2

/// A region of physical memory, as reported by the bootloader and modeled
/// after the Multiboot2 spec.
typedef struct memory_region
{
  uint64_t addr;
  uint64_t len;
  uint32_t type; // one of the `MEMORY_*` types
  uint32_t reserved;
} memory_region_t;

/// The physical memory map: a contiguous array of regions and its length.
typedef struct memory_regions
{
  const memory_region_t* regions;
  uint32_t count;
} memory_regions_t;

/// Physical address ranges that the kernel must keep identity-mapped and that
/// the frame allocator must not hand out.
typedef struct memory_reserved_areas
{
  uint64_t kernel_end;
  uint64_t start;
  uint64_t end;
} memory_reserved_areas_t;

/// The linear framebuffer, when the board has one.
///
/// An `addr` of 0 means there is none.
typedef struct memory_framebuffer
{
  uintptr_t addr;
  uint32_t pitch;
  uint32_t height;
} memory_framebuffer_t;

/// The kernel section is writable.
#define MEMORY_SECTION_WRITABLE 0x1
/// The kernel section holds executable code.
#define MEMORY_SECTION_EXECUTABLE 0x2

/**
 * Called by `memory_map_kernel_sections()` for every allocated section of the
 * kernel image.
 *
 * @param addr the section start address
 * @param size the section size in bytes
 * @param flags a combination of `MEMORY_SECTION_*`
 */
typedef void (*memory_section_map_fn)(uint64_t addr,
                                      uint64_t size,
                                      uint32_t flags);

/**
 * Returns the physical memory map reported by the bootloader.
 *
 * @return the regions and their count
 */
memory_regions_t memory_get_regions();

/**
 * Returns the physical address ranges reserved by the bootloader.
 *
 * @return the reserved address ranges
 */
memory_reserved_areas_t memory_get_reserved_areas();

/**
 * Returns the description of the linear framebuffer.
 *
 * @return the framebuffer description
 */
memory_framebuffer_t memory_get_framebuffer();

/**
 * Invokes `map` for every allocated section of the kernel image, so that
 * paging can identity map each section with the right permissions.
 *
 * @param map the callback to invoke for each section
 */
void memory_map_kernel_sections(memory_section_map_fn map);

#endif
