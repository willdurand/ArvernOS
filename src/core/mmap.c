#include "mmap.h"
#include <core/debug.h>

multiboot_tag_mmap_t *mmap;
physical_address_t multiboot_reserved_start;
physical_address_t multiboot_reserved_end;
int next_free_frame;

void mmap_init(multiboot_tag_mmap_t *mmap, physical_address_t mb_start, physical_address_t mb_end)
{
    mmap = mmap;
    multiboot_reserved_start = mb_start;
    multiboot_reserved_end = mb_end;
    next_free_frame = 1;

    DEBUG(
        "Initialized MMAP with mmap addr = 0x%x, multiboot_reserved_start = 0x%x, "
        "multiboot_reserved_end = 0x%x, next_free_frame = %d\n",
        mmap,
        multiboot_reserved_start,
        multiboot_reserved_end,
        next_free_frame
    );
}

physical_address_t mmap_read(physical_address_t request, uint8_t mode)
{
    // We're reserving frame number 0 for errors, so skip all requests for 0
    if (request == 0) {
        return 0;
    }

    // If the user specifies an invalid mode, also skip the request
    if (mode != MMAP_GET_NUM && mode != MMAP_GET_ADDR) {
        return 0;
    }

    // TODO: implement me

    return 0;
}

physical_address_t mmap_allocate_frame()
{
    // Get the address for the next free frame
    physical_address_t cur_addr = mmap_read(next_free_frame, MMAP_GET_ADDR);

    // Verify that the frame is not in the multiboot reserved area. If it is,
    // increment the next free frame number and recursively call back.
    if (cur_addr >= multiboot_reserved_start && cur_addr <= multiboot_reserved_end) {
        ++next_free_frame;

        return mmap_allocate_frame();
    }

    // Call mmap_read again to get the frame number for our address
    physical_address_t cur_num = mmap_read(cur_addr, MMAP_GET_NUM);

    // Update next_free_frame to the next unallocated frame number
    next_free_frame = cur_num + 1;

    // Finally, return the newly allocated frame num
    return cur_num;
}
