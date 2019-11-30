#ifndef MMU_MMAP_H
#define MMU_MMAP_H

#include <core/boot.h>

#define PAGE_SIZE       4096

#define MMAP_GET_NUM    0
#define MMAP_GET_ADDR   1

typedef uint64_t physical_address_t;
typedef uint64_t frame_t;

void mmap_init(multiboot_tag_mmap_t *mmap, reserved_areas_t reserved);
frame_t mmap_allocate_frame();
physical_address_t mmap_read(frame_t request, uint8_t mode);
frame_t frame_containing_address(physical_address_t addr);
physical_address_t frame_starting_address(frame_t frame);
void mmap_deallocate_frame(frame_t frame);

#endif
