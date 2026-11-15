#include <mmu/memory.h>

#include <stddef.h>

extern char __notes_start, __notes_end;
extern char __text_start, __text_end;
extern char __initcall_start, __initcall_end;
extern char __rodata_start, __rodata_end;
extern char __data_start, __data_end;
extern char __bss_start, __bss_end;

void memory_map_linked_sections(memory_section_map_fn map)
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
