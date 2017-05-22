#ifndef MMU_MMU_H
#define MMU_MMU_H

#include <mmu/mmap.h>
#include <mmu/paging.h>

void mmu_init(multiboot_tag_mmap_t *mmap, reserved_areas_t reserved);

#endif
