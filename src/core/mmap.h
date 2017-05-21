#ifndef CORE_MMU_H
#define CORE_MMU_H

#include <core/boot.h>

#define MMAP_GET_NUM    0
#define MMAP_GET_ADDR   1
#define PAGE_SIZE       4096

typedef uint64_t physical_address_t;

void mmap_init(multiboot_tag_mmap_t *mmap, physical_address_t mb_start, physical_address_t mb_end);
uint32_t mmap_allocate_frame();
physical_address_t mmap_read(uint32_t request, uint8_t mode);


#endif
