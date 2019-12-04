#include "kmain.h"
#include <core/isr.h>
#include <core/timer.h>
#include <core/debug.h>
#include <drivers/screen.h>
#include <kernel/panic.h>
#include <kernel/kshell.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <stdio.h>
#include <string.h>
#include <mem.h>

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
    int len = strlen(msg);

    char buf[SCREEN_WIDTH - 5];
    memcpy(buf, msg, len);
    memset(buf + len * sizeof(char), ' ', SCREEN_WIDTH - 5 - len);

    printf(buf);
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

    print_welcome_messge();

    // enable serial port
    serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
    DEBUG("%s has started", KERNEL_NAME);

    multiboot_info_t* mbi = (multiboot_info_t*) addr;
    reserved_areas_t reserved = read_multiboot_info(mbi);

    // memory
    printf("multiboot_start = 0x%X, multiboot_end = 0x%X\n", reserved.multiboot_start,
           reserved.multiboot_end);
    printf("kernel_start    = 0x%X, kernel_end    = 0x%X\n\n", reserved.kernel_start,
           reserved.kernel_end);

    print_step("initializing interruptions");
    isr_init();
    irq_init();
    print_ok();

    // enable scheduler
    print_step("initializing clock (timer)");
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
