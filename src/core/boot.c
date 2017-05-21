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

    uint64_t kernel_start = -1;
    uint64_t kernel_end = 0;

    DEBUG("announced MBI size 0x%x\n", size);

    for (
        tag = (multiboot_tag_t *) (addr + 8);
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                DEBUG("command line = %s\n", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                DEBUG("boot loader name = %s\n", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                DEBUG(
                    "module at 0x%x-0x%x. command line %s\n",
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
                    "boot device 0x%x,%u,%u\n",
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
                            "mmap base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
                            (unsigned) (mmap->addr >> 32),
                            (unsigned) (mmap->addr & 0xffffffff),
                            (unsigned) (mmap->len >> 32),
                            (unsigned) (mmap->len & 0xffffffff),
                            (unsigned) mmap->type
                        );
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                DEBUG("%s\n", "framebuffer");
                break;
            case MULTIBOOT_TAG_TYPE_APM:
                DEBUG("%s\n", "apm");
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                DEBUG("%s\n", "acpi old");
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                DEBUG("%s\n", "acpi new");
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
                            "elf section #%d addr = 0x%x, type = 0x%X, size = 0x%x, flags = 0x%X\n",
                            i,
                            (unsigned) (elf->addr >> 32),
                            elf->type,
                            (unsigned) (elf->size >> 32),
                            elf->flags
                        );

                        if (elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL) {
                            continue;
                        }

                        if (((uint64_t) (elf->addr)) < kernel_start) {
                            kernel_start = (uint64_t) elf->addr;
                        }

                        if (((uint64_t) (elf->addr)) + elf->size > kernel_end) {
                            kernel_end = (uint64_t) elf->addr;
                            kernel_end += elf->size;
                        }
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_NETWORK:
                DEBUG("%s\n", "network");
                break;
            case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
                DEBUG("%s\n", "load base addr");
                break;

            default:
                DEBUG("tag 0x%x, size 0x%x\n", tag->type, tag->size);
        }
    }

    tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7));
    DEBUG("total MBI size 0x%x\n", (unsigned long) tag - addr);

    DEBUG("kernel start = 0x%x, kernel end = 0x%x\n", (kernel_start >> 32), (kernel_end >> 32));
    DEBUG("multiboot start = 0x%x, multiboot end = 0x%x\n", addr, tag);
}
