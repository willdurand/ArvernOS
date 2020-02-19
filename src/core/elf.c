#include "elf.h"
#include <core/debug.h>
#include <mmu/paging.h>
#include <string.h>

int is_elf(elf_header_t* elf);
void load_segment(uint8_t* data, elf_program_header_t* program_header);
int64_t elf_do_reloc(elf_header_t* hdr, elf_rel_t* rel, elf_section_header_t* reltab);

static inline elf_section_header_t* elf_section_header(elf_header_t* header) {

    return (elf_section_header_t*)((uint64_t)header + header->sh_offset);
}

static inline elf_section_header_t* elf_section(elf_header_t* header, int index) {

    return elf_section_header(header) + index;
}

static inline char* elf_str_table(elf_header_t* header) {

    if (header->strtab_index == ELF_SECTION_INDEX_UNDEFINED) {

        return NULL;
    }

    return (char*)((uint64_t)header + elf_section(header, header->strtab_index)->offset);
}

static inline char* elf_lookup_string(elf_header_t* header, int offset) {

    char* strtab = elf_str_table(header);

    if (strtab == NULL) {

        return NULL;
    }

    return strtab + offset;
}

static void* elf_lookup_symbol(elf_header_t* header, const char* name) {

    // TODO
    return NULL;
}

static int64_t elf_get_symval(elf_header_t* header, int table, uint32_t index) {

    if (table == ELF_SECTION_INDEX_UNDEFINED || index == ELF_SECTION_INDEX_UNDEFINED) {

        return 0;
    }

    elf_section_header_t* symtab = elf_section(header, table);

    uint32_t symtab_entries = symtab->size / symtab->entsize;

    if (index >= symtab_entries) {

        DEBUG("Symbol Index out of Range (%d:%u).\n", table, index);

        return ELF_RELOC_ERR;
    }

    uint64_t symaddr = (uint64_t)header + symtab->offset;

    elf_symbol_t* symbol = &((elf_symbol_t*)symaddr)[index];

    if (symbol->sectionTableIndex == ELF_SECTION_INDEX_UNDEFINED) {

        // External symbol, lookup value
        elf_section_header_t* strtab = elf_section(header, symtab->link);
        const char* name = (const char*)header + strtab->offset + symbol->name;

        void* target = elf_lookup_symbol(header, name);

        if (target == NULL) {

            // Extern symbol not found
            if (ELF_SYMBOL_BIND(symbol->info) & ELF_SYMBOL_BINDING_WEAK) {

                // Weak symbol initialized as 0
                return 0;

            } else {

                DEBUG("Undefined External Symbol : %s.\n", name);

                return ELF_RELOC_ERR;
            }

        } else {

            return (int64_t)target;
        }

    } else if (symbol->sectionTableIndex == ELF_SECTION_INDEX_ABS) {

        // Absolute symbol
        return symbol->value;

    } else {

        // Internally defined symbol
        elf_section_header_t* target = elf_section(header, symbol->sectionTableIndex);

        return (int64_t)header + symbol->value + target->offset;
    }
}

elf_header_t* elf_load(uint8_t* data) {

    elf_header_t* elf = (elf_header_t*)data;

    if (is_elf(elf) != ELF_TYPE_EXECUTABLE) {

        DEBUG("%s", "not an executable");

        return 0;
    }

    DEBUG(
        "file header: machine=%#x version=%#x type=%d entry=%p header_size=%u "
        "ph_size=%u ph_num=%d ph_offset=%llu sh_size=%u sh_num=%d sh_offset=%llu "
        "strtab_index=%d",
        elf->machine, elf->version, elf->type, elf->entry, elf->header_size,
        elf->ph_size, elf->ph_num, elf->ph_offset, elf->sh_size, elf->sh_num,
        elf->sh_offset, elf->strtab_index);

    elf_section_header_t* sections = elf_section_header(elf);

    //We need two phases to load all the sections before loading the program code
    for (uint16_t i = 0; i < elf->sh_num; i++) {

        elf_section_header_t* section = sections + i;

        //Try to get the section's name
        char* name = section->name == 0 ? NULL : elf_lookup_string(elf, section->name);

        if (name == NULL) {

            name = "(None)";
        }

        DEBUG("Phase 1 processing section %d (name: %d (\"%s\"), type: %d, flags: %#lld)", i,
            section->name, name, section->type, section->flags);

        //First we allocate each section that requires allocation and has a valid size
        if (section->flags & ELF_SECTION_FLAG_ALLOC && section->size > 0) {

            void* memory = malloc(section->size);

            DEBUG("Allocated memory for this section (%lld bytes).\n", section->size);

            //If we're progbits, we're supposed to copy the memory in this section
            if (section->type == ELF_SECTION_TYPE_PROGBITS) {

                DEBUG("%s", "Read Progbits for this section");

                memcpy(memory, (void*)((uint64_t)elf + section->offset),
                    section->size);

            } else if (section->type == ELF_SECTION_TYPE_NOBITS) { //If we're nobits, we're supposed to be zero-inited

                DEBUG("%s", "Zero-memory'd this section");

                memset(memory, 0, section->size);
            }

            section->addr = (uint64_t)memory; //Store the local memory for safe-keeping
        }
        else if(section->type == ELF_SECTION_TYPE_SYMTAB || section->type == ELF_SECTION_TYPE_STRTAB) {

            DEBUG("%s", "Loading Symbol or String table");

            elf_symbol_t *table = (elf_symbol_t*)malloc(section->size);

            memcpy(table, data + section->offset, section->size);

            section->addr = (uint64_t)table;
        }
    }

    //Phase 2
    for (uint16_t i = 0; i < elf->sh_num; i++) {

        elf_section_header_t* section = sections + i;

        DEBUG("Phase 2 processing section %d (type: %d)", i, section->type);

        //Handle relocations
        if(section->type == ELF_SECTION_TYPE_REL) {

            int entry_count = section->size / section->entsize;

            if (section->entsize != sizeof(elf_rel_t)) {

                DEBUG("%s", "Section entry size doesn't match rel_t size");

                //TODO: Cleanup
                return NULL;
            }

            DEBUG("Relocating %d elements in REL section", entry_count);

            for (uint64_t index = 0; index < section->size / section->entsize; index++) {

                elf_rel_t* reltab = ((elf_rel_t*)((uint64_t)elf + section->offset)) + index;

                int result = elf_do_reloc(elf, reltab, section);

                if (result == ELF_RELOC_ERR) {

                    DEBUG("%s", "Failed to reloc a symbol.");

                    // TODO: Cleanup
                    return NULL;
                }
            }

        } else if (section->type == ELF_SECTION_TYPE_RELA) {

            int entry_count = section->size / section->entsize;

            if(section->entsize != sizeof(elf_rela_t)) {

                DEBUG("%s", "Section entry size doesn't match rela_t size");

                //TODO: Cleanup
                return NULL;
            }

            DEBUG("Relocating %d elements in RELA section", entry_count);

            elf_rela_t *entries = (elf_rela_t *)malloc(section->size);

            section->addr = (uint64_t)entries;

            memcpy(entries, data + section->offset, section->size);

            //Get the section we're relocating
            elf_section_header_t *relocation_section = elf_section(elf, section->info);
            char *relocation_data = (char *)relocation_section->addr;

            //Get the symbol table for this relocation table
            elf_section_header_t *symbol_table_section = elf_section(elf, section->link);
            elf_symbol_t *symbol_table = (elf_symbol_t *)symbol_table_section->addr;

            //Get the string table for the symbol table
            elf_section_header_t *string_table_section = elf_section(elf, symbol_table_section->link);
            char *string_table = (char *)string_table_section->addr;

            //Relocate all entries
            for(uint64_t index = 0; index < entry_count; index++) {

                elf_rela_t *entry = entries + index;

                //Get the symbol for this entry
                elf_symbol_t *symbol = symbol_table + entry->symbol;
                char *symbol_name = string_table + symbol->name;

                if(entry->type == ELF_REL_TYPE_64) {

                    uint64_t *location = (uint64_t *)relocation_data + entry->offset;

                    if(symbol->sectionTableIndex > 0) {

                        if (symbol->name != 0) {

                            DEBUG("Relocating symbol %s", symbol_name);

                        } else {

                            elf_section_header_t* string_table = elf_section(elf, elf->strtab_index);
                            elf_section_header_t* section = elf_section(elf, symbol->sectionTableIndex);

                            DEBUG("Relocating symbol %s+%lld", ((char*)string_table->addr) + section->name, entry->addend);
                        }

                        //Find the symbol
                        elf_section_header_t* symbol_section = elf_section(elf, symbol->sectionTableIndex);
                        uint64_t symbol_value = symbol_section->addr + symbol->value + entry->addend;

                        //Store the location
                        *location = symbol_value;

                    } else {

                        DEBUG("Unknown symbol %s", symbol_name);

                        //TODO: Cleanup

                        return NULL;
                    }

                } else {

                    DEBUG("Unknown relocation type: %d\n", entry->type);

                    return 1;
                }
            }
        }
    }

    DEBUG("%s", "Phase 3 processing program headers");

    elf_program_header_t* program_headers = (elf_program_header_t*)((uint64_t)data + elf->ph_offset);

    for (uint16_t i = 0; i < elf->ph_num; i++) {

        elf_program_header_t *program_header = program_headers + i;

        DEBUG("program header: type=%d addr=%p", program_header->type,
            program_header->virtual_address);

        if (program_header->type == ELF_PROGRAM_TYPE_LOAD) {

            load_segment(data, program_header);
        }
    }

    DEBUG("elf entry is now at: %p", elf->entry);

    return elf;
}

void elf_unload(elf_header_t* elf) {

    DEBUG("%s", "Unloading an elf file");

    //Note: This does not currently work

    elf_program_header_t* program_headers = (elf_program_header_t*)((uint64_t)elf + elf->ph_offset);

    for (uint16_t i = 0; i < elf->ph_num; i++) {

        elf_program_header_t *program_header = program_headers + i;

        if (program_header->type == ELF_PROGRAM_TYPE_LOAD) {

            DEBUG("Unmapping program header: type=%d addr=%p", program_header->type,
                program_header->virtual_address);

            unmap(page_containing_address(program_header->virtual_address));
        }
    }

    elf_section_header_t* sections = elf_section_header(elf);

    for (uint16_t i = 0; i < elf->sh_num; i++) {

        elf_section_header_t* section = &sections[i];

        if (section->flags & ELF_SECTION_FLAG_ALLOC && section->size > 0) {

            char* name = section->name == 0 ? NULL : elf_lookup_string(elf, section->name);

            if (name == NULL) {

                name = "(None)";
            }

            DEBUG("Freeing section %d (name: %d (\"%s\"), type: %d)", i, section,
                section->name, name, section->type);

            free((void*)section->addr);
        }
    }
}

int is_elf(elf_header_t* elf)
{
    int iself = -1;

    if ((elf->identity[0] == 0x7f) && !strncmp((char*)&elf->identity[1], "ELF", 3)) {

        iself = 0;
    }

    if (iself == 0) {

        DEBUG("%s", "validating elf structs");

        if (elf->header_size != sizeof(elf_header_t)) {

            DEBUG("%s", "Invalid elf header size");

            iself = -1;

        } else if (elf->ph_size != sizeof(elf_program_header_t)) {

            DEBUG("%s", "Invalid program header size");

            iself = -1;

        } else if (elf->sh_size != sizeof(elf_section_header_t)) {

            DEBUG("%s", "Invalid section header size");

            iself = -1;
        }
    }

    if (iself != -1) {

        iself = elf->type;
    }

    return iself;
}

#define R_AMD64_NONE 0 // None
#define R_AMD64_64 1 // word64 (S + A)
#define R_AMD64_PC32 2 // word32 (S + A - P)
#define R_AMD64_GOT32 3 // word32 (G + A)
#define R_AMD64_PLT32 4 // word32 (L + A - P)
#define R_AMD64_COPY 5 // None
#define R_AMD64_GLOB_DAT 6 // word64 (S)
#define R_AMD64_JUMP_SLOT 7 // word64 (S)
#define R_AMD64_RELATIVE 8 // word64 (B + A)
#define R_AMD64_GOTPCREL 9 // word32 (G + GOT + A - P)
#define R_AMD64_32 10 // word32 (S + A)
#define R_AMD64_32S 11 // word32 (S + A)
#define R_AMD64_16 12 // word16 (S + A)
#define R_AMD64_PC16 13 // word16 (S + A - P)
#define R_AMD64_8 14 // word8 (S + A)
#define R_AMD64_PC8 15 // word8 (S + A - P)
#define R_AMD64_PC64 24 // word64 (S + A - P)
#define R_AMD64_GOTOFF64 25 // word64 (S + A - GOT)
#define R_AMD64_GOTPC32 26 // word32 (GOT + A + P)
#define R_AMD64_SIZE32 32 // word32 (Z + A)
#define R_AMD64_SIZE64 33 // word64 (Z + A)

#define DO_AMD64_64(S, A) ((S) + (A))
#define DO_AMD64_32(S, A) (uint32_t)((S) + (A))
#define DO_AMD64_PC64(S, A, P) ((S) + (A) - (P))
#define DO_AMD64_PC32(S, A, P) (uint32_t)((S) + (A) - (P))

int64_t elf_do_reloc(elf_header_t* hdr, elf_rel_t* rel, elf_section_header_t* reltab) {

    elf_section_header_t* target = elf_section(hdr, reltab->info);

    int64_t addr = (int64_t)hdr + target->offset;
    int64_t* ref = (int64_t*)(addr + rel->offset);

    // Symbol value
    int64_t symval = 0;

    if (ELF_R_SYM(rel->info) != ELF_SECTION_INDEX_UNDEFINED) {

        symval = elf_get_symval(hdr, reltab->link, ELF_R_SYM(rel->info));

        if (symval == ELF_RELOC_ERR) {

            return ELF_RELOC_ERR;
        }
    }

    // Relocate based on type
    switch (ELF_R_TYPE(rel->info)) {

    case R_AMD64_NONE:

        // No relocation
        break;

    case R_AMD64_64:

        // Symbol + Offset
        *ref = DO_AMD64_64(symval, *ref);

        break;

    case R_AMD64_32:

        // Symbol + Offset
        *ref = DO_AMD64_32(symval, *ref);

        break;

    case R_AMD64_PC64:

        // Symbol + Offset - Section Offset
        *ref = DO_AMD64_PC64(symval, *ref, (int64_t)ref);

        break;

    case R_AMD64_PC32:

        // Symbol + Offset - Section Offset
        *ref = DO_AMD64_PC32(symval, *ref, (int64_t)ref);

        break;

    default:

        // Relocation type not supported, display error and return
        DEBUG("Unsupported Relocation Type (%d).\n", ELF_R_TYPE(rel->info));

        return ELF_RELOC_ERR;
    }

    return symval;
}

void load_segment(uint8_t* data, elf_program_header_t* program_header) {

    uint64_t mem_size = program_header->mem_size; // Size in memory
    uint64_t file_size = program_header->file_size; // Size in file
    uint64_t addr = program_header->virtual_address; // Offset in memory
    uint64_t offset = program_header->offset; // Offset in file

    uint32_t flags = PAGING_FLAG_PRESENT;

    if (program_header->flags & ELF_PROGRAM_FLAG_W) {

        flags |= PAGING_FLAG_WRITABLE;
    }

    if (!(program_header->flags & ELF_PROGRAM_FLAG_X)) {

        flags |= PAGING_FLAG_NO_EXECUTE;
    }

    DEBUG("load segment at addr=%p with flags=%#x", addr, flags);

    map(page_containing_address(addr), flags);

    if (mem_size == 0) {

        return;
    }

    memcpy((void*)addr, &data[offset], file_size);
    memset((void*)(addr + file_size), 0, mem_size - file_size);
}
