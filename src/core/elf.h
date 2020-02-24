/** @file */
#ifndef CORE_ELF_H
#define CORE_ELF_H

#include <stdint.h>

/// Executable ELF Type.
#define ELF_TYPE_EXECUTABLE 0x2

/// Loadable program type.
#define ELF_PROGRAM_TYPE_LOAD 0x1

/// Execute Program Flag.
#define ELF_PROGRAM_FLAG_EXECUTE 0x1
/// Write Program Flag.
#define ELF_PROGRAM_FLAG_WRITE 0x2
/// Read Program Flag.
#define ELF_PROGRAM_FLAG_READ 0x4

/// Undefined or meaningless section reference.
#define ELF_SECTION_INDEX_UNDEFINED 0

/// Unused section header.
#define ELF_SECTION_TYPE_NULL 0
/// Information defined by the program.
#define ELF_SECTION_TYPE_PROGBITS 1
/// Uninitialized Space (doesn't use any space in the file).
#define ELF_SECTION_TYPE_NOBITS 8

/// Contains writable data.
#define ELF_SECTION_FLAG_WRITE 0x1
/// Section is allocated in memory image of program.
#define ELF_SECTION_FLAG_ALLOC 0x2
/// Contains executable instructions.
#define ELF_SECTION_FLAG_EXECINSTR 0x4

typedef struct elf_header
{
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

typedef struct elf_program_header
{
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  uint64_t virtual_address;
  uint64_t physical_address;
  uint64_t file_size;
  uint64_t mem_size;
  uint64_t align;
} __attribute__((packed)) elf_program_header_t;

typedef struct elf_section_header
{
  uint32_t name;      ///< Section Name
  uint32_t type;      ///< Section Type
  uint64_t flags;     ///< Section attributes
  uint64_t addr;      ///< Virtual Address in memory
  uint64_t offset;    ///< Offset in file
  uint64_t size;      ///< Size of section
  uint32_t link;      ///< Link to other section
  uint32_t info;      ///< Miscellaneous information
  uint64_t addralign; ///< Address alignment boundary
  uint64_t entsize;   ///< Size of entries, if section has table
} __attribute__((packed)) elf_section_header_t;

elf_header_t* elf_load(uint8_t* data);

#endif
