#ifndef MMU_MMAP_H
#define MMU_MMAP_H

#include <core/boot.h>

#define PAGE_SIZE       4096

#define MMAP_GET_NUM    0
#define MMAP_GET_ADDR   1

void mmap_init(multiboot_info_t* mbi);
uint64_t mmap_allocate_frame();
uint64_t mmap_read(uint64_t request, uint8_t mode);
void mmap_deallocate_frame(uint64_t addr);
uint64_t frame_containing_address(uint64_t addr);
uint64_t frame_starting_address(uint64_t addr);

#endif
