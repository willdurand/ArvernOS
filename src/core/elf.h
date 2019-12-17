/** @file */
#ifndef CORE_ELF_H
#define CORE_ELF_H

#include <stdint.h>

#define ELF_TYPE_EXECUTABLE 0x2

#define ELF_PT_LOAD 0x1

#define ELF_PT_X 0x1
#define ELF_PT_R 0x2
#define ELF_PT_W 0x4

typedef struct elf_header {
    unsigned char identity[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t ph_offset;
    uint64_t sh_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_size;
    uint16_t ph_num;
    uint16_t sh_size;
    uint16_t sh_num;
    uint16_t strtab_index;
} __attribute__((packed)) elf_header_t;

typedef struct elf_program_header {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t virtual_address;
    uint64_t physical_address;
    uint64_t file_size;
    uint64_t mem_size;
    uint64_t align;
} __attribute__((packed)) elf_program_header_t;

elf_header_t* elf_load(uint64_t* data);

#endif
