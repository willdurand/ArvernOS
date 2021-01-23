#include "multiboot.h"
#include "logging.h"
#include <stdio.h>

void* find_multiboot_tag(multiboot_info_t* mbi, uint16_t type)
{
  multiboot_tag_t* tag = NULL;

  // `*tag` points to the first tag of the multiboot_info_t struct
  for (tag = (multiboot_tag_t*)mbi->tags; tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (multiboot_tag_t*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
    if (tag->type == type) {
      CORE_DEBUG("found tag for type=%d", type);
      return tag;
    }
  }

  return NULL;
}

reserved_areas_t find_reserved_areas(multiboot_info_t* mbi)
{
  multiboot_tag_t* tag = NULL;
  reserved_areas_t reserved = { .kernel_start = 0,
                                .kernel_end = 0,
                                .multiboot_start = (uint64_t)mbi,
                                .multiboot_end = 0,
                                .start = 0,
                                .end = 0 };

  CORE_DEBUG("announced MBI size %#x", mbi->size);

  for (tag = (multiboot_tag_t*)mbi->tags; tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (multiboot_tag_t*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE:
        CORE_DEBUG("command line=%s", ((multiboot_tag_string_t*)tag)->string);
        break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        CORE_DEBUG("boot loader name=%s",
                   ((multiboot_tag_string_t*)tag)->string);
        break;

      case MULTIBOOT_TAG_TYPE_MODULE:
        CORE_DEBUG("module at %p-%p with command line=%s",
                   ((multiboot_tag_module_t*)tag)->mod_start,
                   ((multiboot_tag_module_t*)tag)->mod_end,
                   ((multiboot_tag_module_t*)tag)->cmdline);
        reserved.end = (uint64_t)((multiboot_tag_module_t*)tag)->mod_end;
        break;

      case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        CORE_DEBUG("mem_lower = %dKB, mem_upper = %dKB",
                   ((multiboot_tag_basic_meminfo_t*)tag)->mem_lower,
                   ((multiboot_tag_basic_meminfo_t*)tag)->mem_upper);
        break;

      case MULTIBOOT_TAG_TYPE_BOOTDEV:
        CORE_DEBUG("boot device %#x, %u, %u",
                   ((multiboot_tag_bootdev_t*)tag)->biosdev,
                   ((multiboot_tag_bootdev_t*)tag)->slice,
                   ((multiboot_tag_bootdev_t*)tag)->part);
        break;

      case MULTIBOOT_TAG_TYPE_MMAP: {
        multiboot_mmap_entry_t* mmap = NULL;

        for (mmap = ((multiboot_tag_mmap_t*)tag)->entries;
             (uint8_t*)mmap < (uint8_t*)tag + tag->size;
             mmap = (multiboot_mmap_entry_t*)((uint64_t)mmap +
                                              ((multiboot_tag_mmap_t*)tag)
                                                ->entry_size)) {
          CORE_DEBUG("mmap base_addr = %p, length = %#x, type = %#x",
                     mmap->addr,
                     mmap->len,
                     mmap->type);
        }
        break;
      }

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
        CORE_DEBUG("%s", "framebuffer");
        break;

      case MULTIBOOT_TAG_TYPE_APM:
        CORE_DEBUG("%s", "apm");
        break;

      case MULTIBOOT_TAG_TYPE_ACPI_OLD:
        CORE_DEBUG("%s", "acpi old");
        break;

      case MULTIBOOT_TAG_TYPE_ACPI_NEW:
        CORE_DEBUG("%s", "acpi new");
        break;

      case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
        multiboot_elf_sections_entry_t* elf = NULL;

        uint32_t i = 0;

        for (
          i = 0, elf = ((multiboot_tag_elf_sections_t*)tag)->sections;
          i < ((multiboot_tag_elf_sections_t*)tag)->num;
          elf =
            (multiboot_elf_sections_entry_t*)((uint64_t)elf +
                                              ((multiboot_tag_elf_sections_t*)
                                                 tag)
                                                ->section_size),
         i++) {
          CORE_DEBUG("elf section #%02d addr = %p, type = %#x, size = %#x, "
                     "flags = %#x",
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
        break;
      }

      case MULTIBOOT_TAG_TYPE_NETWORK:
        CORE_DEBUG("%s", "network");
        break;

      case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
        CORE_DEBUG("%s", "load base addr");
        break;

      default:
        CORE_DEBUG("tag %#x, size %#x", tag->type, tag->size);
    }
  }

  tag = (multiboot_tag_t*)((uint8_t*)tag + ((tag->size + 7) & ~7));
  reserved.multiboot_end = (uint64_t)tag;

  if (reserved.multiboot_end > reserved.end) {
    reserved.end = reserved.multiboot_end;
  }

  CORE_DEBUG("total MBI size %#x", (uint64_t)tag - (uint64_t)mbi);

  reserved.start = reserved.kernel_start;

  CORE_DEBUG("kernel_start=%#x kernel_end=%#x multiboot_start=%#x "
             "multiboot_end=%#x reserved start=%#x end=%#x",
             reserved.kernel_start,
             reserved.kernel_end,
             reserved.multiboot_start,
             reserved.multiboot_end,
             reserved.start,
             reserved.end);

  return reserved;
}
