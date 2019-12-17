#include "elf.h"
#include <core/debug.h>
#include <mmu/paging.h>
#include <mem.h>
#include <string.h>

int is_elf(elf_header_t* elf);
void load_segment(uint64_t* data, elf_program_header_t* program_header);

elf_header_t* elf_load(uint64_t* data) {
    elf_header_t* elf = (elf_header_t*)data;

    if (is_elf(elf) != ELF_TYPE_EXECUTABLE) {
        DEBUG("%s", "not an executable");
        return 0;
    }

    DEBUG(
        "file header: machine=%#x version=%#x entry=%p header_size=%u "
        "ph_size=%u ph_num=%d ph_offset=%u sh_num=%u sh_num=%d",
        elf->machine, elf->version, elf->entry, elf->header_size, elf->ph_size,
        elf->ph_num, elf->ph_offset, elf->sh_size, elf->sh_num
    );

    elf_program_header_t* program_header = (elf_program_header_t*)((uint64_t)data + elf->ph_offset);

    for (uint64_t i = 0; i < elf->ph_num; i++) {
        DEBUG(
            "program header: type=%d addr=%p",
            program_header[0].type,
            program_header[0].virtual_address
        );

        if (program_header[i].type == ELF_PT_LOAD) {
            load_segment(data, &program_header[i]);
        }
    }

    // See below why this is needed. We update the entrypoint here so that it
    // does not have to be updated everywhere else.
    elf->entry += 0x100000;
    DEBUG("elf entry is now at: %p", elf->entry);

    return elf;
}

int is_elf(elf_header_t* elf) {
    int iself = -1;

    if ((elf->identity[0] == 0x7f) && !strncmp((char*)&elf->identity[1], "ELF", 3)) {
        iself = 0;
    }

    if (iself != -1) {
        iself = elf->type;
    }

    return iself;
}

void load_segment(uint64_t* data, elf_program_header_t* program_header) {
    uint64_t mem_size = program_header->mem_size;      // Size in memory
    uint64_t file_size = program_header->file_size;    // Size in file
    uint64_t addr = program_header->virtual_address;   // Offset in memory
    uint64_t offset = program_header->offset;          // Offset in file

    uint32_t flags = PAGING_FLAG_PRESENT;

    if (program_header->flags & ELF_PT_W) {
        flags |= PAGING_FLAG_WRITABLE;
    }

    // This is a hack to be able to map the segment, otherwise the page might
    // be already used. Note that this offset does not guarantee anything, it
    // happens to work with it, though.
    addr += 0x100000;

    DEBUG("load segment at addr=%p (modified) with flags=%#x", addr, flags);

    map(page_containing_address(addr), flags);

    if (mem_size == 0) {
        return;
    }

    memcpy(addr, &data[offset], file_size);
    memset(addr + file_size, 0, mem_size - file_size);
}
