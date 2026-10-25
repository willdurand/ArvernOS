#include <memory.h>

#include <arvern/utils.h>
#include <core/pvh.h>

extern char __kernel_start;
extern char __kernel_end;

static memory_area_t areas[1];

memory_regions_t memory_get_regions()
{
  pvh_mmap_t* mmap = pvh_get_mmap();

  // The PVH memory map is E820-compatible, so it is passed through as-is.
  return (memory_regions_t){
    .regions = (const memory_region_t*)mmap->addr,
    .count = mmap->count,
  };
}

memory_reserved_areas_t memory_get_reserved_areas()
{
  uintptr_t module_start = 0;
  uintptr_t module_end = 0;
  pvh_get_modules_range(&module_start, &module_end);

  // The bootloader does not account for the areas it has populated on top of
  // plain RAM, such as the boot modules (e.g. the init ramdisk), which QEMU
  // places at the top of the available memory.
  uint32_t area_count = 0;
  if (module_end > module_start) {
    areas[0] = (memory_area_t){
      .start = module_start,
      .end = module_end,
      .flags = MEMORY_AREA_NO_ALLOC | MEMORY_AREA_IDENTITY,
    };
    area_count = 1;
  }

  return (memory_reserved_areas_t){
    .kernel_start = (uintptr_t)&__kernel_start,
    .kernel_end = (uintptr_t)&__kernel_end,
    .areas = areas,
    .area_count = area_count,
  };
}

memory_framebuffer_t memory_get_framebuffer()
{
  return (memory_framebuffer_t){ .addr = 0, .pitch = 0, .height = 0 };
}

void memory_map_kernel_sections(memory_section_map_fn map)
{
  UNUSED(map);
}
