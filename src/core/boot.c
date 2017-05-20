#include "boot.h"
#include <stdio.h>
#include <core/debug.h>

int multiboot_is_valid(unsigned long magic, unsigned long addr)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%x\n", magic);
        return -1;
    }

    if (addr & 7) {
        printf("Unaligned MBI: 0x%x\n", addr);
        return -2;
    }

    return 0;
}

void dump_multiboot_info(unsigned long addr)
{
    multiboot_tag_t *tag;
    unsigned size = *(unsigned *) addr;

    DEBUG("Announced MBI size 0x%x\n", size);

    for (
        tag = (multiboot_tag_t *) (addr + 8);
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                DEBUG("Command line = %s\n", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                DEBUG("Boot loader name = %s\n", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                DEBUG(
                    "Module at 0x%x-0x%x. Command line %s\n",
                    ((multiboot_tag_module_t *) tag)->mod_start,
                    ((multiboot_tag_module_t *) tag)->mod_end,
                    ((multiboot_tag_module_t *) tag)->cmdline
                );
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                DEBUG(
                    "mem_lower = %dKB, mem_upper = %dKB\n",
                    ((multiboot_tag_basic_meminfo_t *) tag)->mem_lower,
                    ((multiboot_tag_basic_meminfo_t *) tag)->mem_upper
                );
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                DEBUG(
                    "Boot device 0x%x,%u,%u\n",
                    ((multiboot_tag_bootdev_t *) tag)->biosdev,
                    ((multiboot_tag_bootdev_t *) tag)->slice,
                    ((multiboot_tag_bootdev_t *) tag)->part
                );
                break;

            default:
                DEBUG("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
        }
    }
}
