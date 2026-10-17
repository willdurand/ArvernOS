#include <core/multiboot.h>
#include <memory.h>
#include <stddef.h>

memory_regions_t memory_get_regions()
{
  multiboot_tag_mmap_t* mmap = multiboot_get_mmap();
  if (mmap == NULL) {
    return (memory_regions_t){ 0 };
  }

  return (memory_regions_t){
    .regions = (const memory_region_t*)mmap->entries,
    .count = (mmap->size - sizeof(multiboot_tag_mmap_t)) / mmap->entry_size,
  };
}

memory_reserved_areas_t memory_get_reserved_areas()
{
  reserved_areas_t areas = multiboot_get_reserved_areas();

  return (memory_reserved_areas_t){
    .kernel_end = areas.kernel_end,
    .start = areas.start,
    .end = areas.end,
  };
}

memory_framebuffer_t memory_get_framebuffer()
{
  multiboot_tag_framebuffer_t* framebuffer = multiboot_get_framebuffer();
  if (framebuffer == NULL || framebuffer->common.framebuffer_addr == 0xB8000) {
    return (memory_framebuffer_t){ .addr = 0, .pitch = 0, .height = 0 };
  }

  return (memory_framebuffer_t){
    .addr = framebuffer->common.framebuffer_addr,
    .pitch = framebuffer->common.framebuffer_pitch,
    .height = framebuffer->common.framebuffer_height,
  };
}

void memory_map_kernel_sections(memory_section_map_fn map)
{
  multiboot_tag_elf_sections_t* sections =
    multiboot_find(MULTIBOOT_TAG_TYPE_ELF_SECTIONS);
  if (sections == NULL) {
    return;
  }

  multiboot_elf_sections_entry_t* elf = sections->sections;
  for (uint32_t i = 0; i < sections->num;
       elf = (multiboot_elf_sections_entry_t*)((uint64_t)elf +
                                               sections->section_size),
                i++) {
    if (elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL ||
        (elf->flags & MULTIBOOT_ELF_SECTION_FLAG_ALLOCATED) !=
          MULTIBOOT_ELF_SECTION_FLAG_ALLOCATED) {
      continue;
    }

    uint32_t flags = 0;
    if (elf->flags & MULTIBOOT_ELF_SECTION_FLAG_WRITABLE) {
      flags |= MEMORY_SECTION_WRITABLE;
    }
    if (elf->flags & MULTIBOOT_ELF_SECTION_FLAG_EXECUTABLE) {
      flags |= MEMORY_SECTION_EXECUTABLE;
    }

    map(elf->addr, elf->size, flags);
  }
}
