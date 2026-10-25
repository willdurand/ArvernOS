#include "multiboot.h"

#include <core/logging.h>
#include <string.h>

void* _multiboot_find(multiboot_tag_t* tag, uint16_t type);

static multiboot_info_t* saved_mbi = NULL;

void multiboot_init(multiboot_info_t* mbi)
{
  saved_mbi = mbi;
}

multiboot_tag_framebuffer_t* multiboot_get_framebuffer()
{
  return multiboot_find(MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
}

void multiboot_get_boot_info(boot_info_t* info)
{
  info->initrd_addr = 0;
  info->initrd_size = 0;
  info->cmdline = NULL;

  multiboot_tag_string_t* string = multiboot_find(MULTIBOOT_TAG_TYPE_CMDLINE);
  if (string != NULL) {
    info->cmdline = string->string;
  }

  // TODO: Add support for more than one module. While most tags are unique in
  // Multiboot2, this isn't the case for modules.
  multiboot_tag_module_t* module = multiboot_find(MULTIBOOT_TAG_TYPE_MODULE);

  if (module != NULL && strcmp(module->cmdline, "initrd") == 0) {
    info->initrd_addr = (uintptr_t)module->mod_start;
    info->initrd_size = module->mod_end - module->mod_start;
  }
}

multiboot_info_t* multiboot_get_info()
{
  return saved_mbi;
}

multiboot_tag_mmap_t* multiboot_get_mmap()
{
  return multiboot_find(MULTIBOOT_TAG_TYPE_MMAP);
}

void* multiboot_find(uint16_t type)
{
  // The tags live in the multiboot information structure, which only exists
  // after `multiboot_init()`.
  if (saved_mbi == NULL) {
    return NULL;
  }

  return _multiboot_find(saved_mbi->tags, type);
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
