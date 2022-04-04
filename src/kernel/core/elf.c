#include <core/elf.h>
#include <core/logging.h>
#include <inttypes.h>
#include <string.h>

#ifdef __x86_64__
#include <mmu/paging.h>
#endif

int elf_get_type(elf_header_t* elf);
void elf_load_segment(uint8_t* data, elf_program_header_t* program_header);

elf_header_t* elf_load(uint8_t* data)
{
  elf_header_t* elf = (elf_header_t*)data;

  if (elf_get_type(elf) != ELF_TYPE_EXECUTABLE) {
    CORE_DEBUG("%s", "not an executable");
    return NULL;
  }

  CORE_DEBUG(
    "file header: machine=%#x version=%#x type=%d entry=%p header_size=%u"
    " ph_size=%u ph_num=%d sh_size=%u sh_num=%d strtab_index=%d",
    elf->machine,
    elf->version,
    elf->type,
    elf->entry,
    elf->header_size,
    elf->ph_size,
    elf->ph_num,
    elf->sh_size,
    elf->sh_num,
    elf->strtab_index);

  elf_program_header_t* program_header =
    (elf_program_header_t*)((uintptr_t)data + elf->ph_offset);

  for (uint16_t i = 0; i < elf->ph_num; i++) {
    CORE_DEBUG("program header: type=%" PRIu32 " addr=%p",
               program_header[i].type,
               program_header[i].virtual_address);

    if (program_header[i].type == ELF_PROGRAM_TYPE_LOAD) {
      elf_load_segment(data, &program_header[i]);
    }
  }

  CORE_DEBUG("elf entry is now at: %p", elf->entry);

  return elf;
}

int elf_get_type(elf_header_t* elf)
{
  int iself = -1;

  if ((elf->identity[0] == 0x7f) &&
      !strncmp((char*)&elf->identity[1], "ELF", 3)) {
    iself = 0;
  }

  if (iself == 0) {
    CORE_DEBUG("%s", "validating elf structs");

    if (elf->header_size != sizeof(elf_header_t)) {
      CORE_DEBUG("invalid elf header size: %d, expected: %d",
                 elf->header_size,
                 sizeof(elf_header_t));

      iself = -1;
    } else if (elf->ph_size != sizeof(elf_program_header_t)) {
      CORE_DEBUG("%s", "invalid program header size");

      iself = -1;
    }
  }

  if (iself != -1) {
    iself = elf->type;
  }

  return iself;
}

void elf_load_segment(uint8_t* data, elf_program_header_t* program_header)
{
  ElfN_Size mem_size = program_header->mem_size;    // Size in memory
  ElfN_Size file_size = program_header->file_size;  // Size in file
  ElfN_Addr addr = program_header->virtual_address; // Address in memory
  ElfN_Off offset = program_header->offset;         // Offset in file

  if (mem_size == 0) {
    return;
  }

#ifdef __x86_64__
  // We need WRITABLE because we copy data right after.
  uint32_t flags =
    PAGING_FLAG_PRESENT | PAGING_FLAG_USER_ACCESSIBLE | PAGING_FLAG_WRITABLE;

  if ((program_header->flags & ELF_PROGRAM_FLAG_EXECUTE) !=
      ELF_PROGRAM_FLAG_EXECUTE) {
    flags |= PAGING_FLAG_NO_EXECUTE;
  }

  CORE_DEBUG("load segment at addr=%p with flags=%#x", addr, flags);

  uint64_t start_page = page_containing_address(addr);
  uint32_t number_of_pages = paging_amount_for_byte_size(addr, mem_size);

  map_multiple(start_page, number_of_pages, flags);
#endif

  memcpy((void*)addr, data + offset, file_size);
  memset((void*)(addr + file_size), 0, mem_size - file_size);

  // TODO: We probably need to drop the WRITABLE flag if the segment isn't
  // supposed to be writable.
}

void elf_unload(elf_header_t* elf)
{
  elf_program_header_t* program_header =
    (elf_program_header_t*)((uintptr_t)elf + elf->ph_offset);

  for (uint16_t i = 0; i < elf->ph_num; i++) {
    if (program_header[i].type == ELF_PROGRAM_TYPE_LOAD) {
      ElfN_Size mem_size = program_header[i].mem_size;

      if (mem_size == 0) {
        continue;
      }

#ifdef __x86_64__
      uint64_t addr = program_header[i].virtual_address;
      uint64_t start_page = page_containing_address(addr);
      uint32_t number_of_pages = paging_amount_for_byte_size(addr, mem_size);

      unmap_multiple(start_page, number_of_pages);
#endif
    }
  }

  CORE_DEBUG("%s", "unloaded elf");
}
