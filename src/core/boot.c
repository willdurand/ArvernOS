#include "boot.h"
#include <stdio.h>
#include <core/debug.h>

bool multiboot_is_valid(unsigned long magic, unsigned long addr)
{
    if (magic != MULTIBOOT2_MAGIC_VALUE) {
        printf("Invalid magic value: 0x%x", magic);
        return false;
    }

    if (addr & 7) {
        printf("Unaligned MBI: 0x%x", addr);
        return false;
    }

    return true;
}

void* find_multiboot_tag(multiboot_tag_t *tags, uint16_t type)
{
    multiboot_tag_t *tag;

    for (
        tag = tags; // points to the first tag of the multiboot_info_t struct
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
        if (tag->type == type) {
            return tag;
        }
    }

    return 0;
}

reserved_areas_t read_multiboot_info(multiboot_info_t *mbi)
{
    multiboot_tag_t *tag;
    reserved_areas_t reserved = {
        .kernel_start = -1,
        .kernel_end = 0,
        .multiboot_start = (uint64_t) mbi,
        .multiboot_end = 0
    };

    DEBUG("announced MBI size 0x%x", mbi->size);

    for (
        tag = (multiboot_tag_t *) mbi->tags;
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                DEBUG("command line = %s", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                DEBUG("boot loader name = %s", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                DEBUG(
                    "module at 0x%x-0x%x. command line %s",
                    ((multiboot_tag_module_t *) tag)->mod_start,
                    ((multiboot_tag_module_t *) tag)->mod_end,
                    ((multiboot_tag_module_t *) tag)->cmdline
                );
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                DEBUG(
                    "mem_lower = %dKB, mem_upper = %dKB",
                    ((multiboot_tag_basic_meminfo_t *) tag)->mem_lower,
                    ((multiboot_tag_basic_meminfo_t *) tag)->mem_upper
                );
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                DEBUG(
                    "boot device 0x%x,%u,%u",
                    ((multiboot_tag_bootdev_t *) tag)->biosdev,
                    ((multiboot_tag_bootdev_t *) tag)->slice,
                    ((multiboot_tag_bootdev_t *) tag)->part
                );
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    multiboot_mmap_entry_t *mmap;
                    for (
                        mmap = ((multiboot_tag_mmap_t *) tag)->entries;
                        (uint8_t *) mmap < (uint8_t *) tag + tag->size;
                        mmap = (multiboot_mmap_entry_t *) ((unsigned long) mmap + ((multiboot_tag_mmap_t *) tag)->entry_size)
                    ) {
                        DEBUG(
                            "mmap base_addr = 0x%X, length = 0x%X, type = 0x%x",
                            mmap->addr,
                            mmap->len,
                            mmap->type
                        );
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                DEBUG("%s", "framebuffer");
                break;
            case MULTIBOOT_TAG_TYPE_APM:
                DEBUG("%s", "apm");
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                DEBUG("%s", "acpi old");
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                DEBUG("%s", "acpi new");
                break;
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                {
                    multiboot_elf_sections_entry_t *elf;

                    uint32_t i;
                    for (
                        i = 0,
                        elf = ((multiboot_tag_elf_sections_t *) tag)->sections;
                        i < ((multiboot_tag_elf_sections_t *) tag)->num;
                        elf = (multiboot_elf_sections_entry_t *) ((uint64_t) elf + ((multiboot_tag_elf_sections_t *) tag)->section_size),
                        i++
                    ) {
                        DEBUG(
                            "elf section #%d addr = 0x%X, type = 0x%X, size = 0x%X, flags = 0x%X",
                            i,
                            elf->addr,
                            elf->type,
                            elf->size,
                            elf->flags
                        );

                        if (elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL) {
                            continue;
                        }

                        if (((uint64_t) (elf->addr)) < reserved.kernel_start) {
                            reserved.kernel_start = (uint64_t) elf->addr;
                        }

                        if (((uint64_t) (elf->addr)) + elf->size > reserved.kernel_end) {
                            reserved.kernel_end = (uint64_t) elf->addr;
                            reserved.kernel_end += elf->size;
                        }
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_NETWORK:
                DEBUG("%s", "network");
                break;
            case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
                DEBUG("%s", "load base addr");
                break;

            default:
                DEBUG("tag 0x%x, size 0x%x", tag->type, tag->size);
        }
    }

    tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7));
    reserved.multiboot_end = (uint64_t) tag;

    DEBUG("total MBI size 0x%X", (uint64_t) tag - (uint64_t) mbi);

    return reserved;
}
