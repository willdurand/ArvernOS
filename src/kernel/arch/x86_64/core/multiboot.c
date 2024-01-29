#include "multiboot.h"
#include <core/logging.h>
#include <stdio.h>

reserved_areas_t build_reserved_areas();
void* _multiboot_find(multiboot_tag_t* tag, uint16_t type);

static multiboot_info_t* saved_mbi = NULL;
static reserved_areas_t reserved_areas = { .kernel_start = 0,
                                           .kernel_end = 0,
                                           .multiboot_start = 0,
                                           .multiboot_end = 0,
                                           .start = 0,
                                           .end = 0 };
static const char* cmdline = NULL;
static multiboot_tag_framebuffer_t* framebuffer = NULL;
static multiboot_tag_mmap_t* mmap = NULL;

void multiboot_init(multiboot_info_t* mbi)
{
  saved_mbi = mbi;
  reserved_areas = build_reserved_areas();
  framebuffer = multiboot_find(MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
  mmap = multiboot_find(MULTIBOOT_TAG_TYPE_MMAP);

  multiboot_tag_string_t* string = multiboot_find(MULTIBOOT_TAG_TYPE_CMDLINE);
  if (string != NULL) {
    cmdline = string->string;
  }
}

multiboot_tag_framebuffer_t* multiboot_get_framebuffer()
{
  return framebuffer;
}

const char* multiboot_get_cmdline()
{
  return cmdline;
}

reserved_areas_t multiboot_get_reserved_areas()
{
  return reserved_areas;
}

multiboot_info_t* multiboot_get_info()
{
  return saved_mbi;
}

multiboot_tag_mmap_t* multiboot_get_mmap()
{
  return mmap;
}

void* multiboot_find(uint16_t type)
{
  return _multiboot_find(saved_mbi->tags, type);
}

reserved_areas_t build_reserved_areas()
{
  reserved_areas_t reserved = { .kernel_start = 0,
                                .kernel_end = 0,
                                .multiboot_start = (uint64_t)saved_mbi,
                                .multiboot_end =
                                  (uint64_t)saved_mbi + saved_mbi->size,
                                .start = 0,
                                .end = 0 };

  multiboot_tag_elf_sections_t* sections =
    multiboot_find(MULTIBOOT_TAG_TYPE_ELF_SECTIONS);
  if (sections != NULL) {
    uint32_t i = 0;
    multiboot_elf_sections_entry_t* elf = sections->sections;
    for (; i < sections->num;
         elf = (multiboot_elf_sections_entry_t*)((uint64_t)elf +
                                                 sections->section_size),
         i++) {
      CORE_DEBUG("elf section #%02d: addr=%p, type=%#x, size=%#x, flags=%#x",
                 i,
                 elf->addr,
                 elf->type,
                 elf->size,
                 elf->flags);

      if (elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL) {
        continue;
      }

      if (elf->addr < reserved.kernel_start || reserved.kernel_start == 0) {
        reserved.kernel_start = elf->addr;
      }

      if ((elf->addr + elf->size) > reserved.kernel_end) {
        reserved.kernel_end = elf->addr + elf->size;
      }
    }
  }

  reserved.start = reserved.kernel_start;

  // TODO: Add support for more modules. While most tags are unique in
  // Multiboot2, this isn't the case for modules.
  multiboot_tag_module_t* module = multiboot_find(MULTIBOOT_TAG_TYPE_MODULE);
  if (module != NULL) {
    CORE_DEBUG("module: cmdline=%s", module->cmdline);
    reserved.end = module->mod_end;
  }

  // Adjust end in case we don't have any modules.
  if (reserved.end < reserved.multiboot_end) {
    reserved.end = reserved.multiboot_end;
  }

  CORE_DEBUG("kernel_start=%#x kernel_end=%#x multiboot_start=%#x "
             "multiboot_end=%#x reserved_start=%#x reserved_end=%#x",
             reserved.kernel_start,
             reserved.kernel_end,
             reserved.multiboot_start,
             reserved.multiboot_end,
             reserved.start,
             reserved.end);

  return reserved;
}

void* _multiboot_find(multiboot_tag_t* first_tag, uint16_t type)
{
  multiboot_tag_t* tag = NULL;
  for (tag = first_tag; tag->type != MULTIBOOT_TAG_TYPE_END;
       // Round up to 8-byte boundary as per section "3.1.3 General tag
       // structure" of the Multiboot2 spec.
       tag = (multiboot_tag_t*)((
         uint8_t*)(((uintptr_t)tag + tag->size + (8 - 1)) & -8))) {
    if (tag->type == type) {
      CORE_DEBUG("found tag for type=%d", type);
      return tag;
    }
  }

  CORE_DEBUG("could not find tag for type=%d", type);
  return NULL;
}
