#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

/* The magic field should contain this. */
#define MULTIBOOT2_HEADER_MAGIC			0xe85250d6

/* This should be in %eax. */
#define MULTIBOOT2_BOOTLOADER_MAGIC		0x36d76289

/* Flags set in the 'flags' member of the multiboot header.  */
#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_BOOTDEV           5
#define MULTIBOOT_TAG_TYPE_MMAP              6
#define MULTIBOOT_TAG_TYPE_VBE               7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MULTIBOOT_TAG_TYPE_APM               10
#define MULTIBOOT_TAG_TYPE_EFI32             11
#define MULTIBOOT_TAG_TYPE_EFI64             12
#define MULTIBOOT_TAG_TYPE_SMBIOS            13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD          14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW          15
#define MULTIBOOT_TAG_TYPE_NETWORK           16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP          17
#define MULTIBOOT_TAG_TYPE_EFI_BS            18
#define MULTIBOOT_TAG_TYPE_EFI32_IH          19
#define MULTIBOOT_TAG_TYPE_EFI64_IH          20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

#define MULTIBOOT_MEMORY_AVAILABLE  1
#define MULTIBOOT_MEMORY_RESERVED   2

#define MULTIBOOT_ELF_SECTION_TYPE_NULL 0

typedef struct multiboot_header
{
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
    // tags?
} multiboot_header_t;

typedef struct multiboot_header_tag {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
} multiboot_header_tag_t;

typedef struct multiboot_tag {
    uint32_t type;
    uint32_t size;
} multiboot_tag_t;

typedef struct multiboot_tag_string {
    uint32_t type;
    uint32_t size;
    char string[];
} multiboot_tag_string_t;

typedef struct multiboot_tag_basic_meminfo {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
} multiboot_tag_basic_meminfo_t;

typedef struct multiboot_tag_bootdev {
    uint32_t type;
    uint32_t size;
    uint32_t biosdev;
    uint32_t slice;
    uint32_t part;
} multiboot_tag_bootdev_t;

typedef struct multiboot_tag_module {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline[];
} multiboot_tag_module_t;

typedef struct multiboot_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed)) multiboot_mmap_entry_t;

typedef struct multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot_mmap_entry_t entries[];
} multiboot_tag_mmap_t;

typedef struct multiboot_tag_network {
    uint32_t type;
    uint32_t size;
    uint8_t dhcpack[];
} multiboot_tag_network_t;

typedef struct multiboot_elf_sections_entry {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t alignment;
    uint64_t entry_size;
} __attribute__((packed)) multiboot_elf_sections_entry_t;

typedef struct multiboot_tag_elf_sections {
    uint32_t type;
    uint32_t size;
    uint32_t num;
    uint32_t section_size;
    uint32_t shndx;
    multiboot_elf_sections_entry_t sections[];
} multiboot_tag_elf_sections_t;

int multiboot_is_valid(unsigned long magic, unsigned long addr);
void dump_multiboot_info(unsigned long addr);
void* find_multiboot_tag(unsigned long addr, uint16_t type);
uint64_t get_kernel_start();
uint64_t get_kernel_end();
uint64_t get_multiboot_start();
uint64_t get_multiboot_end();

#endif
