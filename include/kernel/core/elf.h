/**
 * @file
 * @see https://www.uclibc.org/docs/elf-64-gen.pdf
 * @see https://man7.org/linux/man-pages/man5/elf.5.html
 *
 * This header file contains the definitions of the Executable and Linking
 * Format (ELF) for both 32 (arm) and 64 bits (the default).
 */
#ifndef CORE_ELF_H
#define CORE_ELF_H

#include <stdint.h>

/// Executable ELF Type.
#define ELF_TYPE_EXECUTABLE 0x2

/// Loadable program type.
#define ELF_PROGRAM_TYPE_LOAD 0x1

/// Inactive section with undefined values.
#define ELF_SECTION_TYPE_NULL 0x0
/// Section data contains executable code and data.
#define ELF_SECTION_TYPE_PROGBITS 0x1
/// Section data contains a symbol table.
#define ELF_SECTION_TYPE_SYMTAB 0x2
/// Section data contains a string table
#define ELF_SECTION_TYPE_STRTAB 0x3
/// Section data occupies no space in the file.
#define ELF_SECTION_TYPE_NOBITS 0x8

/// Execute Program Flag.
#define ELF_PROGRAM_FLAG_EXECUTE 0x1
/// Write Program Flag.
#define ELF_PROGRAM_FLAG_WRITE 0x2
/// Read Program Flag.
#define ELF_PROGRAM_FLAG_READ 0x4

/// Identifies a section that occupies memory during process execution.
#define ELF_SECTION_FLAG_ALLOC 0x2

#ifdef __arm__

#define ElfN_Addr uint32_t
#define ElfN_Off  uint32_t
#define ElfN_Size uint32_t

#else // __arm__

#define ElfN_Addr uint64_t
#define ElfN_Off  uint64_t
#define ElfN_Size uint64_t

#endif

typedef struct elf_header
{
  unsigned char identity[16];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  ElfN_Addr entry;
  ElfN_Off ph_offset;
  ElfN_Off sh_offset;
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
#ifndef __arm__
  uint32_t flags;
#endif
  ElfN_Off offset;
  ElfN_Addr virtual_address;
  ElfN_Addr physical_address;
  ElfN_Size file_size;
  ElfN_Size mem_size;
#ifdef __arm__
  ElfN_Size flags;
#endif
  ElfN_Size align;
} __attribute__((packed)) elf_program_header_t;

typedef struct elf_section_header
{
  uint32_t name;             ///< Section name
  uint32_t type;             ///< Section type
  ElfN_Size flags;           ///< Section attributes
  ElfN_Addr virtual_address; ///< Virtual address in memory
  ElfN_Off offset;           ///< Offset in file
  ElfN_Size size;            ///< Size of section
  uint32_t link;             ///< Link to other section
  uint32_t info;             ///< Miscellaneous information
  ElfN_Size addralign;       ///< Address alignment boundary
  ElfN_Size entsize;         ///< Size of entries, if section has table
} __attribute__((packed)) elf_section_header_t;

elf_header_t* elf_load(uint8_t* data);

void elf_unload(elf_header_t* elf);

#endif
