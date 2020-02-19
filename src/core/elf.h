/** @file */
#ifndef CORE_ELF_H
#define CORE_ELF_H

#include <stdint.h>
#include <stdlib.h>

#define ELF_TYPE_NONE               0 //No file type
#define ELF_TYPE_REL                1 //Relocatable object file
#define ELF_TYPE_EXECUTABLE         2 //Executable file
#define ELF_TYPE_DYN                3 //Shared object file
#define ELF_TYPE_CORE               4 //Core file
#define ELF_TYPE_LOOS               0xFE00 //Environment-specific use
#define ELF_TYPE_HIOS               0xFEFF //Environment-specific use
#define ELF_TYPE_LOPROC             0xFF00 //Processor-specific use
#define ELF_TYPE_HIPROC             0xFFFF //Processor-specific use

#define ELF_PROGRAM_TYPE_NULL       0 //Unused entry
#define ELF_PROGRAM_TYPE_LOAD       1 //Loadable segment
#define ELF_PROGRAM_TYPE_DYNAMIC    2 //Dynamic linking tables
#define ELF_PROGRAM_TYPE_INTERP     3 //Program interpreter path name
#define ELF_PROGRAM_TYPE_NOTE       4 //Note sections
#define ELF_PROGRAM_TYPE_SHLIB      5 //Reserved
#define ELF_PROGRAM_TYPE_PHDR       6 //Program header table
#define ELF_PROGRAM_TYPE_LOOS       0x60000000 //Environment-specific use
#define ELF_PROGRAM_TYPE_HIOS       0x6FFFFFFF //Environment-specific use
#define ELF_PROGRAM_TYPE_LOPROC     0x70000000 //Processor-specific use
#define ELF_PROGRAM_TYPE_HIPROC     0x7FFFFFFF //Processor-specific use

#define ELF_PROGRAM_FLAG_X          0x1 //Execute permission
#define ELF_PROGRAM_FLAG_W          0x2 //Write permission
#define ELF_PROGRAM_FLAG_R          0x4 //Read permission
#define ELF_PROGRAM_FLAG_MASKOS     0x00FF0000 //Reserved for enviroment-specific use
#define ELF_PROGRAM_FLAG_MASKPROC   0xFF000000 //Reserved for processor-specific use

#define ELF_ID_MAG0                 0 //File identification
#define ELF_ID_MAG1                 1 //File identification
#define ELF_ID_MAG2                 2 //File identification
#define ELF_ID_MAG3                 3 //File identification
#define ELF_ID_CLASS                4 //File Class
#define ELF_ID_DATA                 5 //Data Encoding
#define ELF_ID_VERSION              6 //File Version
#define ELF_ID_OSABI                7 //OS/ABI Identification
#define ELF_ID_ABIVERSION           8 //ABI Version
#define ELF_ID_PAD                  9 //Start of padding bytes
#define ELF_ID_NIDENT               16 //Size of identity

#define ELF_CLASS32                 1 //32-bit object
#define ELF_CLASS64                 2 //64-bit object

#define ELF_DATA_2LSB               1 //Little-endian
#define ELF_DATA_2MSB               2 //Big-endian

#define ELF_SECTION_INDEX_UNDEFINED 0 //Undefined or meaningless section reference
#define ELF_SECTION_INDEX_LOPROC    0xFF00 //Processor-specific use
#define ELF_SECTION_INDEX_HIPROC    0xFF1F //Processor-specific use
#define ELF_SECTION_INDEX_LOOS      0xFF20 //Environment-specific use
#define ELF_SECTION_INDEX_HIOS      0xFF3F //Environment-specific use
#define ELF_SECTION_INDEX_ABS       0xFFF1 //Absolute value
#define ELF_SECTION_INDEX_COMMON    0xFFF2 //Common block

#define ELF_SECTION_TYPE_NULL       0 //Unused section header
#define ELF_SECTION_TYPE_PROGBITS   1 //Information defined by the program
#define ELF_SECTION_TYPE_SYMTAB     2 //Linker Symbol Table
#define ELF_SECTION_TYPE_STRTAB     3 //String Table
#define ELF_SECTION_TYPE_RELA       4 //Rela-type relocation entries
#define ELF_SECTION_TYPE_HASH       5 //Symbol hash table
#define ELF_SECTION_TYPE_DYNAMIC    6 //Dynamic Linking Tables
#define ELF_SECTION_TYPE_NOTE       7 //Note Information
#define ELF_SECTION_TYPE_NOBITS     8 //Uninitialized Space, doesn't use any space in the file
#define ELF_SECTION_TYPE_REL        9 //Rel-type relocation entries
#define ELF_SECTION_TYPE_SHLIB      10 //Reserved
#define ELF_SECTION_TYPE_DYNSYM     11 //Dynamic Loader Symbol Table
#define ELF_SECTION_TYPE_LOOS       0x60000000 //Environment-specific use
#define ELF_SECTION_TYPE_HOOS       0x6FFFFFFF //Environment-specific use
#define ELF_SECTION_TYPE_LOPROC     0x70000000 //Processor-specific use
#define ELF_SECTION_TYPE_HIPROC     0x7FFFFFFF //Processor-specific use

#define ELF_SECTION_FLAG_WRITE      0x1 //Contains writable data
#define ELF_SECTION_FLAG_ALLOC      0x2 //Section is allocated in memory image of program
#define ELF_SECTION_FLAG_EXECINSTR  0x4 //Contains executable instructions
#define ELF_SECTION_FLAG_MASKOS     0x0F000000 //Environment-specific use
#define ELF_SECTION_FLAG_MASKPROC   0xF0000000 //Processor-specific use

#define ELF_SYMBOL_BINDING_LOCAL    0 //Not visible outside of the object file
#define ELF_SYMBOL_BINDING_GLOBAL   1 //Global symbol
#define ELF_SYMBOL_BINDING_WEAK     2 //Global scope, but lower precedence than global symbols
#define ELF_SYMBOL_BINDING_LOOS     10 //Environment-specific use
#define ELF_SYMBOL_BINDING_HIOS     12 //Environment-specific use
#define ELF_SYMBOL_BINDING_LOPROC   13 //Processor-specific use
#define ELF_SYMBOL_BINDING_HIPROC   15 //Processor-specific use

#define ELF_SYMBOL_TYPE_NOTYPE      0 //No type specified (e.g., absolute symbol)
#define ELF_SYMBOL_TYPE_OBJECT      1 //Data Object
#define ELF_SYMBOL_TYPE_FUNC        2 //Function entry point
#define ELF_SYMBOL_TYPE_SECTION     3 //Associated with a section
#define ELF_SYMBOL_TYPE_FILE        4 //Source file associated with the object
#define ELF_SYMBOL_TYPE_LOOS        10 //Environment-specific use
#define ELF_SYMBOL_TYPE_HIOS        12 //Environment-specific use
#define ELF_SYMBOL_TYPE_LOPROC      13 //Processor-specific use
#define ELF_SYMBOL_TYPE_HIPROC      15 //Processor-specific use

#define ELF_DYNAMIC_TABLE_TYPE_NULL     0 //End of dynamic array
#define ELF_DYNAMIC_TABLE_TYPE_NEEDED   1 //String table offset of a needed library (val)
#define ELF_DYNAMIC_TABLE_TYPE_PLTRELSZ 2 //Size, in bytes, of the relocation entries associated with the process linkage (val)
#define ELF_DYNAMIC_TABLE_TYPE_PLTGOT   3 //Address associated with the linkage table. Specific meaning is processor-dependent. (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_HASH     4 //Address of the symbol hash table (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_STRTAB   5 //Address of the dynamic string table (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_SYMTAB   6 //Address of the dynamic symbol table (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_RELA     7 //Address of a relocation table with Rela entries (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_RELASZ   8 //Size, in bytes, of the Rela relocation table (val)
#define ELF_DYNAMIC_TABLE_TYPE_RELAENT  9 //Size, in bytes, of each Rela relocation entry (val)
#define ELF_DYNAMIC_TABLE_TYPE_STRSZ    10 //Size, in bytes, of the string table (val)
#define ELF_DYNAMIC_TABLE_TYPE_SYMENT   11 //Size, in bytes, of each symbol entry (val)
#define ELF_DYNAMIC_TABLE_TYPE_INIT     12 //Address of the initialization function (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_FINI     13 //Address of the termination function (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_SONAME   14 //String table offset to the name of this shared object (val)
#define ELF_DYNAMIC_TABLE_TYPE_RPATH    15 //String table offset to the shared library search path string (val)

//The presence of this dynamic table entry modifies the
//symbol resolution algorithm for references within the
//library. Symbols defined within the library are used to
//resolve references before the dynamic linker searches the
//usual search path.
#define ELF_DYNAMIC_TABLE_TYPE_SYMBOLIC 16

#define ELF_DYNAMIC_TABLE_TYPE_REL      17 //Address of a relocation table with Rel entries (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_RELSZ    18 //Size, in bytes, of the Rel relocation table (val)
#define ELF_DYNAMIC_TABLE_TYPE_RELENT   19 //Size, in bytes, of each Rel relocation entry (val)
#define ELF_DYNAMIC_TABLE_TYPE_PLTREL   20 //Type of relocation entry used for the procedure linkage table. The val member contains either TABLE_TYPE_REL or TABLE_TYPE_RELA.
#define ELF_DYNAMIC_TABLE_TYPE_DEBUG    21 //Reserved for debug usage (ptr)

//The presence of this dynamic table entry signals that the
//relocation table contains relocations for a non-writable
//segment.
#define ELF_DYNAMIC_TABLE_TYPE_TEXTREL  22

#define ELF_DYNAMIC_TABLE_TYPE_JMPREL   23 //Address of the relocations associated with the procedure linkage table (ptr)

//The presence of this dynamic table entry signals that the
//dynamic loader should process all relocations for this object
//before transferring control to the program.
#define ELF_DYNAMIC_TABLE_TYPE_BIND_NOW 24

#define ELF_DYNAMIC_TABLE_TYPE_INIT_ARRAY   25 //Pointer to an array of pointers to initialization functions (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_FINI_ARRAY   26 //Pointer to an array of pointers to termination functions (ptr)
#define ELF_DYNAMIC_TABLE_TYPE_INIT_ARRAYSZ 27 //Size, in bytes, of the array of initialization functions (val)
#define ELF_DYNAMIC_TABLE_TYPE_FINI_ARRAYSZ 28 //Size, in bytes, of the array of termination functions (val)
#define ELF_DYNAMIC_TABLE_TYPE_LOOS         0x60000000 //Defines a range of dynamic table tags that are reserved for environment-specific use
#define ELF_DYNAMIC_TABLE_TYPE_HIOS         0x6FFFFFFF //Defines a range of dynamic table tags that are reserved for environment-specific use
#define ELF_DYNAMIC_TABLE_TYPE_LOPROC       0x70000000 //Defines a range of dynamic table tags that are reserved for processor-specific use.
#define ELF_DYNAMIC_TABLE_TYPE_HIPROC       0x7FFFFFFF //Defines a range of dynamic table tags that are reserved for processor-specific use.

#define ELF_R_SYM(i) ((i) >> 32)
#define ELF_R_TYPE(i) ((i) & 0xFFFFFFFFL)
#define ELF_R_INFO(s, t) (((s) << 32) + ((t) & 0xFFFFFFFFL))

#define ELF_SYMBOL_BIND(INFO)	((INFO) >> 4)
#define ELF_SYMBOL_TYPE(INFO)	((INFO) & 0x0F)

#define ELF_RELOC_ERR -1

#define ELF_REL_TYPE_64 1

typedef struct elf_header {

    unsigned char identity[16]; //ELF Identification
    uint16_t type; //Object file type
    uint16_t machine; //Machine type
    uint32_t version; //Object file version
    uint64_t entry; //Entry point address
    uint64_t ph_offset; //Program header offset
    uint64_t sh_offset; //Section header offset
    uint32_t flags; //Processor-specific flags
    uint16_t header_size; //ELF Header size
    uint16_t ph_size; //Size of program header entry
    uint16_t ph_num; //Number of program header entries
    uint16_t sh_size; //Size of section header entry
    uint16_t sh_num; //Number of section header entries
    uint16_t strtab_index; //Section Name String Table Index

} __attribute__((packed)) elf_header_t;

typedef struct elf_section_header {

    uint32_t name; //Section Name
    uint32_t type; //Section Type
    uint64_t flags; //Section attributes
    uint64_t addr; //Virtual Address in memory
    uint64_t offset; //Offset in file
    uint64_t size; //Size of section
    uint32_t link; //Link to other section
    uint32_t info; //Miscellaneous information
    uint64_t addralign; //Address alignment boundary
    uint64_t entsize; //Size of entries, if section has table

} __attribute__((packed)) elf_section_header_t;

typedef struct elf_symbol {

    uint32_t name; //Symbol name
    uint8_t info; //Type and binding attributes
    uint8_t other; //Reserved
    uint16_t sectionTableIndex; //Section table index
    uint64_t value; //Symbol value
    uint64_t size; //Size of object

} __attribute__((packed)) elf_symbol_t;

typedef struct elf_rel {

    uint64_t offset; //Address of reference
    uint64_t info; //Symbol Index and type of relocation
} __attribute__((packed)) elf_rel_t;

typedef struct elf_rela {

    uint64_t offset; //Address of reference
    uint32_t type; //Type of relocation
    uint32_t symbol; //Symbol index
    int64_t addend; //Constant part of expression

} __attribute__((packed)) elf_rela_t;

typedef struct elf_program_header {

    uint32_t type; //Type of segment
    uint32_t flags; //Segment Attributes
    uint64_t offset; //Offset in file
    uint64_t virtual_address; //Virtual address in memory
    uint64_t physical_address; //Reserved
    uint64_t file_size; //Size of segment in file
    uint64_t mem_size; //Size of segment in memory
    uint64_t align; //Alignment of segment

} __attribute__((packed)) elf_program_header_t;

typedef struct elf_dyn {

    int64_t tag; //Type of dynamic entry
    union {

        uint64_t value; //Represents int values
        uint64_t ptr; //Represents virtual addresses
    } un;

} __attribute__((packed)) elf_dyn_t;

elf_header_t* elf_load(uint8_t* data);

void elf_unload(elf_header_t *elf);
#endif
