#include "mmu.h"

void mmu_init(multiboot_tag_mmap_t* mmap, reserved_areas_t reserved) {
    mmap_init(mmap, reserved);
    paging_init();
}
