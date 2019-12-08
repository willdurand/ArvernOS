#include "kmain.h"
#include <core/debug.h>
#include <core/isr.h>
#include <core/syscall.h>
#include <core/timer.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <kernel/kshell.h>
#include <kernel/panic.h>
#include <mmu/mmap.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>

void print_welcome_messge();
void print_step(const char* msg);
void print_ok();
void check_interrupts();

void print_welcome_messge() {
    screen_color_scheme(COLOR_LIGHT_BLUE, COLOR_BLACK);
    printf("%s\n", KERNEL_ASCII);
    screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
    printf("%s %s / Built on: %s at %s\n\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE, KERNEL_TIME);
}

void print_step(const char* msg) {
    printf("%-75s", msg);
}

void print_ok() {
    printf("[");
    screen_color_scheme(COLOR_GREEN, COLOR_BLACK);
    printf("OK");
    screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
    printf("]\n");
}

void check_interrupts() {
    print_step("checking interrupts");

    uint32_t tick = timer_tick();

    while (tick == timer_tick()) ;

    print_ok();
}

void kmain(unsigned long magic, unsigned long addr) {
    screen_init();
    screen_clear();

    if (multiboot_is_valid(magic, addr) == false) {
        PANIC("invalid multiboot");
    }

    multiboot_info_t* mbi = (multiboot_info_t*) addr;

    print_welcome_messge();

    // enable serial port
    serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
    DEBUG("%s has started", KERNEL_NAME);

    print_step("initializing frame allocator");
    mmap_init(mbi);
    print_ok();

    print_step("initializing interruptions");
    isr_init();
    irq_init();
    print_ok();

    print_step("initializing syscalls");
    syscall_init();
    print_ok();

    print_step("initializing clock/timer");
    timer_init();
    print_ok();

    check_interrupts();

    print_step("initializing keyboard");
    keyboard_init();
    print_ok();

    // shell
    printf("\n");
    print_prompt();

    while (1) {
        kshell(keyboard_get_last_scancode());
        __asm__("hlt");
    }
}
