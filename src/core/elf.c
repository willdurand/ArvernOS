#include "elf.h"
#include "logging.h"
#include <mmu/paging.h>
#include <string.h>

int get_elf_type(elf_header_t* elf);
void load_segment(uint8_t* data, elf_program_header_t* program_header);

elf_header_t* elf_load(uint8_t* data)
{
  elf_header_t* elf = (elf_header_t*)data;

  if (get_elf_type(elf) != ELF_TYPE_EXECUTABLE) {
<<<<<<< HEAD
    DEBUG_OUT("%s", "not an executable");
    return 0;
  }

  DEBUG_OUT(
=======
    CORE_DEBUG("%s", "not an executable");
    return 0;
  }

  CORE_DEBUG(
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74
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

  elf_program_header_t* program_header =
    (elf_program_header_t*)((uint64_t)data + elf->ph_offset);

  for (uint64_t i = 0; i < elf->ph_num; i++) {
<<<<<<< HEAD
    DEBUG_OUT("program header: type=%d addr=%p",
              program_header[i].type,
              program_header[i].virtual_address);
=======
    CORE_DEBUG("program header: type=%d addr=%p",
               program_header[i].type,
               program_header[i].virtual_address);
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74

    if (program_header[i].type == ELF_PROGRAM_TYPE_LOAD) {
      load_segment(data, &program_header[i]);
    }
  }

<<<<<<< HEAD
  DEBUG_OUT("elf entry is now at: %p", elf->entry);
=======
  CORE_DEBUG("elf entry is now at: %p", elf->entry);
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74

  return elf;
}

int get_elf_type(elf_header_t* elf)
{
  int iself = -1;

  if ((elf->identity[0] == 0x7f) &&
      !strncmp((char*)&elf->identity[1], "ELF", 3)) {
    iself = 0;
  }

  if (iself == 0) {
<<<<<<< HEAD
    DEBUG_OUT("%s", "validating elf structs");

    if (elf->header_size != sizeof(elf_header_t)) {
      DEBUG_OUT("%s", "invalid elf header size");

      iself = -1;
    } else if (elf->ph_size != sizeof(elf_program_header_t)) {
      DEBUG_OUT("%s", "invalid program header size");
=======
    CORE_DEBUG("%s", "validating elf structs");

    if (elf->header_size != sizeof(elf_header_t)) {
      CORE_DEBUG("%s", "invalid elf header size");

      iself = -1;
    } else if (elf->ph_size != sizeof(elf_program_header_t)) {
      CORE_DEBUG("%s", "invalid program header size");
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74

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
  uint32_t flags =
    PAGING_FLAG_PRESENT | PAGING_FLAG_USER_ACCESSIBLE | PAGING_FLAG_WRITABLE;

  if (!(program_header->flags & ELF_PROGRAM_FLAG_EXECUTE)) {
    flags |= PAGING_FLAG_NO_EXECUTE;
  }

<<<<<<< HEAD
  DEBUG_OUT("load segment at addr=%p with flags=%#x", addr, flags);
=======
  CORE_DEBUG("load segment at addr=%p with flags=%#x", addr, flags);
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74

  uint64_t start_page = page_containing_address(addr);
  uint32_t number_of_pages = paging_amount_for_byte_size(addr, mem_size);

  if (mem_size == 0) {
    return;
  }

  map_multiple(start_page, number_of_pages, flags);

  memcpy((void*)addr, data + offset, file_size);
  memset((void*)(addr + file_size), 0, mem_size - file_size);

  // TODO: we probably need to drop the WRITABLE flag if the segment isn't
  // supposed to be writable.
}

void elf_unload(elf_header_t* elf)
{
  elf_program_header_t* program_header =
    (elf_program_header_t*)((uint64_t)elf + elf->ph_offset);

  for (uint64_t i = 0; i < elf->ph_num; i++) {
    if (program_header[i].type == ELF_PROGRAM_TYPE_LOAD) {
      uint64_t mem_size = program_header[i].mem_size;
      uint64_t addr = program_header[i].virtual_address;

      uint64_t start_page = page_containing_address(addr);
      uint32_t number_of_pages = paging_amount_for_byte_size(addr, mem_size);

      if (mem_size == 0) {
        continue;
      }

      unmap_multiple(start_page, number_of_pages);
    }
  }

<<<<<<< HEAD
  DEBUG_OUT("%s", "unloaded elf");
=======
  CORE_DEBUG("%s", "unloaded elf");
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74
}
