#include <memory.h>

#include <arvern/utils.h>
#include <core/pvh.h>
#include <stddef.h>

extern char __kernel_start, __kernel_end;
extern char __notes_start, __notes_end;
extern char __text_start, __text_end;
extern char __initcall_start, __initcall_end;
extern char __rodata_start, __rodata_end;
extern char __data_start, __data_end;
extern char __bss_start, __bss_end;

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
  typedef struct kernel_region
  {
    uintptr_t start;
    uintptr_t end;
    uint32_t flags;
  } kernel_region_t;

  static const kernel_region_t regions[] = {
    {
      (uintptr_t)&__notes_start,
      (uintptr_t)&__notes_end,
      0,
    },
    {
      (uintptr_t)&__text_start,
      (uintptr_t)&__text_end,
      MEMORY_SECTION_EXECUTABLE,
    },
    {
      (uintptr_t)&__initcall_start,
      (uintptr_t)&__initcall_end,
      MEMORY_SECTION_WRITABLE,
    },
    {
      (uintptr_t)&__rodata_start,
      (uintptr_t)&__rodata_end,
      0,
    },
    {
      (uintptr_t)&__data_start,
      (uintptr_t)&__data_end,
      MEMORY_SECTION_WRITABLE,
    },
    {
      (uintptr_t)&__bss_start,
      (uintptr_t)&__bss_end,
      MEMORY_SECTION_WRITABLE,
    },
  };

  for (size_t i = 0; i < sizeof(regions) / sizeof(regions[0]); i++) {
    const kernel_region_t* region = &regions[i];

    map(region->start, region->end - region->start, region->flags);
  }
}
