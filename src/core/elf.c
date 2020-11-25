#include "elf.h"
#include <core/debug.h>
#include <mmu/paging.h>
#include <string.h>

int is_elf(elf_header_t* elf);
void load_segment(uint8_t* data, elf_program_header_t* program_header);
elf_section_header_t* get_elf_section_header(elf_header_t* header);
elf_section_header_t* get_elf_section(elf_header_t* header, int index);
char* elf_str_table(elf_header_t* header);
char* elf_lookup_string(elf_header_t* header, int offset);

elf_header_t* elf_load(uint8_t* data)
{
  elf_header_t* elf = (elf_header_t*)data;

  if (is_elf(elf) != ELF_TYPE_EXECUTABLE) {
    DEBUG("%s", "not an executable");
    return 0;
  }

  DEBUG(
    "file header: machine=%#x version=%#x type=%d entry=%p header_size=%u "
    "ph_size=%u ph_num=%d ph_offset=%llu sh_size=%u sh_num=%d sh_offset=%llu "
    "strtab_index=%d",
    elf->machine,
    elf->version,
    elf->type,
    elf->entry,
    elf->header_size,
    elf->ph_size,
    elf->ph_num,
    elf->ph_offset,
    elf->sh_size,
    elf->sh_num,
    elf->sh_offset,
    elf->strtab_index);

  elf_section_header_t* sections = get_elf_section_header(elf);

  // We need two phases to load all the sections before loading the program
  // code. For now we only do one.
  for (uint16_t i = 0; i < elf->sh_num; i++) {
    elf_section_header_t* section = sections + i;

    // Try to get the section's name
    char* name =
      section->name == 0 ? NULL : elf_lookup_string(elf, section->name);

    if (name == NULL) {
      name = "(none)";
    }

    DEBUG("phase 1 processing section %d (name: %d (\"%s\"), type: %d, flags: "
          "%#lld, size: %d)",
          i,
          section->name,
          name,
          section->type,
          section->flags,
          section->size);

    if (section->type == ELF_SECTION_TYPE_NOTE) {
      DEBUG("ignoring note section %d (%d - \"%s\")", i, section->name, name);

      continue;
    }

    // First we allocate each section that requires allocation and has a valid
    // size.
    if ((section->flags & ELF_SECTION_FLAG_ALLOC) && section->size > 0) {
      uint64_t start_page = page_containing_address(section->addr);
      uint32_t number_of_pages =
        paging_amount_for_byte_size(section->addr, section->size);

      uint32_t flags = PAGING_FLAG_PRESENT;

      if ((section->flags & ELF_SECTION_FLAG_WRITE) ||
          section->type == ELF_SECTION_TYPE_PROGBITS ||
          section->type == ELF_SECTION_TYPE_NOBITS) {
        flags |= PAGING_FLAG_WRITABLE;
      }

      if (!(section->flags & ELF_SECTION_FLAG_EXECINSTR)) {
        flags |= PAGING_FLAG_NO_EXECUTE;
      }

      map_multiple(start_page, number_of_pages, flags);

      DEBUG(
        "allocated memory for this section (%lld bytes) (number_of_pages=%d)",
        section->size,
        number_of_pages);

      // If we're progbits, we're supposed to copy the memory in this section
      if (section->type == ELF_SECTION_TYPE_PROGBITS) {
        void* dest = (void*)section->addr;
        void* src = (void*)((uint64_t)elf + section->offset);

        DEBUG("copying progbits for this section: dest=%p src=%p size=%lld",
              dest,
              src,
              section->size);

        memcpy(dest, src, section->size);
      } else if (section->type ==
                 ELF_SECTION_TYPE_NOBITS) { // If we're nobits, we're supposed
                                            // to be zero-init'ed.
        DEBUG("%s", "zero-memory'd this section");

        memset((void*)section->addr, 0, section->size);
      }
    }
  }

  elf_program_header_t* program_header =
    (elf_program_header_t*)((uint64_t)data + elf->ph_offset);

  for (uint64_t i = 0; i < elf->ph_num; i++) {
    DEBUG("program header: type=%d addr=%p",
          program_header[i].type,
          program_header[i].virtual_address);

    if (program_header[i].type == ELF_PROGRAM_TYPE_LOAD) {
      load_segment(data, &program_header[i]);
    }
  }

  DEBUG("elf entry is now at: %p", elf->entry);

  return elf;
}

int is_elf(elf_header_t* elf)
{
  int iself = -1;

  if ((elf->identity[0] == 0x7f) &&
      !strncmp((char*)&elf->identity[1], "ELF", 3)) {
    iself = 0;
  }

  if (iself == 0) {
    DEBUG("%s", "validating elf structs");

    if (elf->header_size != sizeof(elf_header_t)) {
      DEBUG("%s", "invalid elf header size");

      iself = -1;
    } else if (elf->ph_size != sizeof(elf_program_header_t)) {
      DEBUG("%s", "invalid program header size");

      iself = -1;
    }
  }

  if (iself != -1) {
    iself = elf->type;
  }

  return iself;
}

void load_segment(uint8_t* data, elf_program_header_t* program_header)
{
  uint64_t mem_size = program_header->mem_size;    // Size in memory
  uint64_t file_size = program_header->file_size;  // Size in file
  uint64_t addr = program_header->virtual_address; // Offset in memory
  uint64_t offset = program_header->offset;        // Offset in file

  // We need WRITABLE because we copy data right after.
  uint32_t flags = PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;

  if (!(program_header->flags & ELF_PROGRAM_FLAG_EXECUTE)) {
    flags |= PAGING_FLAG_NO_EXECUTE;
  }

  DEBUG("load segment at addr=%p with flags=%#x", addr, flags);

  uint64_t start_page = page_containing_address(addr);
  uint32_t number_of_pages = paging_amount_for_byte_size(addr, mem_size);

  if (mem_size == 0) {
    return;
  }

  map_multiple(start_page, number_of_pages, flags);

  memcpy((void*)addr, data + offset, file_size);
  memset((void*)(addr + file_size), 0, mem_size - file_size);
}

elf_section_header_t* get_elf_section_header(elf_header_t* header)
{
  return (elf_section_header_t*)((uint64_t)header + header->sh_offset);
}

elf_section_header_t* get_elf_section(elf_header_t* header, int index)
{
  return get_elf_section_header(header) + index;
}

char* elf_str_table(elf_header_t* header)
{
  if (header->strtab_index == ELF_SECTION_INDEX_UNDEFINED) {
    return NULL;
  }

  return (char*)((uint64_t)header +
                 get_elf_section(header, header->strtab_index)->offset);
}

char* elf_lookup_string(elf_header_t* header, int offset)
{
  char* strtab = elf_str_table(header);

  if (strtab == NULL) {
    return NULL;
  }

  return strtab + offset;
}

void elf_unload(elf_header_t* elf)
{
  elf_section_header_t* sections = get_elf_section_header(elf);

  for (uint16_t i = 0; i < elf->sh_num; i++) {
    elf_section_header_t* section = sections + i;

    // Try to get the section's name
    char* name = section->name == 0 ? 0 : elf_lookup_string(elf, section->name);

    if (name == 0) {
      name = "(none)";
    }

    if (section->type == ELF_SECTION_TYPE_NOTE) {
      DEBUG("ignoring note section %d (%d - \"%s\")", i, section->name, name);

      continue;
    }

    DEBUG("unloading section %d (name: %d (\"%s\")", i, section->name, name);

    if ((section->flags & ELF_SECTION_FLAG_ALLOC) && section->size > 0) {
      uint64_t start_page = page_containing_address(section->addr);
      uint32_t number_of_pages =
        paging_amount_for_byte_size(section->addr, section->size);

      unmap_multiple(start_page, number_of_pages);
    }
  }

  elf_program_header_t* program_header =
    (elf_program_header_t*)((uint64_t)elf + elf->ph_offset);

  for (uint64_t i = 0; i < elf->ph_num; i++) {
    if (program_header[i].type == ELF_PROGRAM_TYPE_LOAD) {
      uint64_t mem_size = program_header[i].mem_size;
      uint64_t addr = program_header[i].virtual_address;

      uint64_t start_page = page_containing_address(addr);
      uint32_t number_of_pages = paging_amount_for_byte_size(addr, mem_size);

      unmap_multiple(start_page, number_of_pages);
    }
  }

  DEBUG("%s", "unloaded elf");
}
