/**
 * @file
 *
 * Physical memory handoff between the board's bootloader and the MMU.
 *
 * A board learns the physical memory layout from its bootloader and normalizes
 * it here, so that the architecture's frame allocator and paging code stay
 * board-agnostic.
 *
 * - `memory_get_regions()` returns the bootloader's E820-style memory map,
 *   i.e. what the machine says is RAM and what it says is not.
 * - `memory_get_reserved_areas()` returns the ranges the bootloader has
 *   populated on top of plain RAM (kernel image, boot payloads, boot
 *   information), which the E820 memory map does not account for.
 */
#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <stdint.h>

/// Address where the kernel image is loaded, i.e. the end of low memory. Must
/// match the load address set by the board's linker script.
#define KERNEL_LOAD_ADDRESS 0x00100000

/// Usable RAM (E820 type 1).
#define MEMORY_AVAILABLE 1
/// Memory reserved (E820 type 2).
#define MEMORY_RESERVED 2

/// A region of physical memory, as reported by the bootloader's E820-style
/// memory map.
///
/// The map is passed through as-is. Only regions of type `MEMORY_AVAILABLE`
/// are allocatable; all other types are considered unavailable.
typedef struct memory_region
{
  uint64_t addr;
  uint64_t len;
  uint32_t type; // one of the `MEMORY_*` types
  uint32_t reserved;
} memory_region_t;

/// The physical memory map: a contiguous array of regions and its length.
///
/// The array is owned by the bootloader and remains valid for the lifetime of
/// the kernel.
typedef struct memory_regions
{
  const memory_region_t* regions;
  uint32_t count;
} memory_regions_t;

/// The frame allocator must not hand out frames in this area.
#define MEMORY_AREA_NO_ALLOC (1 << 0)
/// This area must remain identity-mapped when paging is enabled.
#define MEMORY_AREA_IDENTITY (1 << 1)

/// A physical address range populated by the bootloader on top of plain RAM
/// (e.g. the init ramdisk), which the E820 memory map does not account for.
typedef struct memory_area
{
  uintptr_t start; // inclusive
  uintptr_t end;   // exclusive
  uint32_t flags;  // combination of the `MEMORY_AREA_*` flags
} memory_area_t;

/// Ranges the kernel must keep identity-mapped and the frame allocator must
/// not hand out: the kernel image and the bootloader-owned `areas` around it.
///
/// Areas may share a page with a kernel section but must not overlap it or
/// each other, and must not be empty.
typedef struct memory_reserved_areas
{
  uintptr_t kernel_start;
  uintptr_t kernel_end;
  const memory_area_t* areas;
  uint32_t area_count;
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
 * Returns the physical address ranges reserved by the bootloader: the kernel
 * image and the areas populated on top of plain RAM around it.
 *
 * The `areas` array is owned by the board and remains valid until the next
 * call to this function.
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
 * The implementation must report page-granular ranges and sections must not
 * overlap each other.
 *
 * @param map the callback to invoke for each section
 */
void memory_map_kernel_sections(memory_section_map_fn map);

#endif
