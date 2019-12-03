#include "kmain.h"
#include <core/isr.h>
#include <core/timer.h>
#include <core/debug.h>
#include <core/check.h>
#include <kernel/panic.h>
#include <kernel/kshell.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <stdio.h>

void print_welcome_messge() {
    printf("%s\n", KERNEL_ASCII);
    printf("%s %s / Built on: %s at %s\n\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE, KERNEL_TIME);
}

void kmain(unsigned long magic, unsigned long addr) {
    screen_init();
    screen_clear();

    if (multiboot_is_valid(magic, addr) == false) {
        PANIC("invalid multiboot");
    }

    print_welcome_messge();

    // enable serial port
    serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
    DEBUG("%s has started", KERNEL_NAME);

    multiboot_info_t* mbi = (multiboot_info_t*) addr;
    reserved_areas_t reserved = read_multiboot_info(mbi);

    // memory
    printf("- multiboot_start = 0x%X, multiboot_end = 0x%X\n", reserved.multiboot_start,
           reserved.multiboot_end);
    printf("- kernel_start    = 0x%X, kernel_end    = 0x%X\n", reserved.kernel_start,
           reserved.kernel_end);

    isr_init();
    irq_init();
    printf("- interruptions enabled\n");

    // enable scheduler
    timer_init();
    printf("- clock (timer) enabled\n");

    // self-checks
    check_interrupts();

    keyboard_init();
    printf("- keyboard routine enabled\n");

    // shell
    printf(PROMPT);

    while (1) {
        kshell(keyboard_get_last_scancode());
        __asm__("hlt");
    }
}
