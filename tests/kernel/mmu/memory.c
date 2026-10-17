/**
 * @file
 *
 * Stub of the board memory handoff for the host unit tests.
 */
#include <memory.h>

memory_regions_t memory_get_regions()
{
  return (memory_regions_t){ 0 };
}

memory_reserved_areas_t memory_get_reserved_areas()
{
  return (memory_reserved_areas_t){ 0 };
}

memory_framebuffer_t memory_get_framebuffer()
{
  return (memory_framebuffer_t){ 0 };
}

void memory_map_kernel_sections(memory_section_map_fn map)
{
  // noop implementation
}
