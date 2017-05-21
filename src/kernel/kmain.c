#include <core/isr.h>
#include <core/timer.h>
#include <core/debug.h>
#include <core/mmap.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <stdio.h>
#include <stdint.h>
#include "kmain.h"

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

void kmain(unsigned long magic, unsigned long addr)
{
    screen_init();
    screen_clear();

    if (multiboot_is_valid(magic, addr) != 0) {
        printf("System halted!");
        return;
    }

    printf("%s\n", KERNEL_ASCII);
    printf("%s %s / Built on: %s at %s\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE, KERNEL_TIME);

    isr_init();
    irq_init();

    // enable scheduler
    timer_init(50);

    // enable serial port
    serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
    DEBUG("%s has started\n", KERNEL_NAME);

    keyboard_init();

    dump_multiboot_info(addr);

    // mmap
    multiboot_tag_mmap_t *mmap = find_multiboot_tag(addr, MULTIBOOT_TAG_TYPE_MMAP);
    mmap_init(mmap, addr, (addr + *(unsigned *) addr));

    uint32_t new_frame = mmap_allocate_frame();
    uint32_t new_frame_addr = mmap_read(new_frame, MMAP_GET_ADDR);
    printf("New frame allocated at: 0x%x\n", new_frame_addr);

    while (1) {
        __asm__("hlt");
    }
}
