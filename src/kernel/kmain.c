#include "kmain.h"
#include <core/cmos.h>
#include <core/debug.h>
#include <core/elf.h>
#include <core/isr.h>
#include <core/syscall.h>
#include <core/timer.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/serial.h>
#include <fs/debug.h>
#include <fs/proc.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <kernel/kshell.h>
#include <kernel/panic.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <stdio.h>
#include <string.h>

void print_welcome_messge();
void print_step(const char* msg);
void print_ok();
void check_interrupts();

void print_welcome_messge()
{
  screen_color_scheme(COLOR_LIGHT_BLUE, COLOR_BLACK);
  printf("%s\n", KERNEL_ASCII);
  screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
  printf("%s %s (%s) / Built on: %s at %s\n\n",
         KERNEL_NAME,
         KERNEL_VERSION,
         GIT_HASH,
         KERNEL_DATE,
         KERNEL_TIME);
}

void print_step(const char* msg)
{
  printf("%-74s", msg);
}

void print_ok()
{
  printf("  [");
  screen_color_scheme(COLOR_GREEN, COLOR_BLACK);
  printf("OK");
  screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
  printf("]");
}

void print_ko()
{
  printf("[");
  screen_color_scheme(COLOR_RED, COLOR_BLACK);
  printf("FAIL");
  screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
  printf("]");
}

void check_interrupts()
{
  print_step("checking interrupts");

  uint32_t tick = timer_tick();

  while (tick == timer_tick()) {
    ;
  }

  print_ok();
}

void kmain(uint64_t addr)
{
  screen_init();
  screen_clear();

  multiboot_info_t* mbi = (multiboot_info_t*)addr;

  print_welcome_messge();

  // enable serial port
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
  DEBUG("%s has started", KERNEL_NAME);

  print_step("initializing interruptions");
  isr_init();
  irq_init();
  print_ok();

  print_step("initializing frame allocator");
  frame_init(mbi);
  print_ok();

  print_step("initializing paging");
  paging_init(mbi);
  print_ok();

  print_step("initializing heap allocator");
  alloc_init();
  print_ok();

  print_step("initializing syscalls");
  syscall_init();
  print_ok();

  print_step("initializing real time clock (cmos)");
  cmos_init();
  print_ok();

  print_step("initializing timer");
  timer_init();
  print_ok();

  check_interrupts();

  print_step("initializing keyboard");
  keyboard_init();
  print_ok();

  print_step("initializing virtual file system");
  vfs_init();
  print_ok();

  print_step("mounting all file systems");
  multiboot_tag_module_t* module =
    find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MODULE);
  inode_t initrd = vfs_mount("/", tar_fs_init((uint64_t)module->mod_start));

  if (initrd) {
    vfs_mount(FS_DEBUG_MOUNTPOINT, debug_fs_init());
    vfs_mount(FS_PROC_MOUNTPOINT, proc_fs_init());
    print_ok();
  } else {
    print_ko();
  }

  // kshell
  printf("\n");
  kshell_print_prompt();

  while (1) {
    kshell_run(keyboard_get_scancode());
    // This allows the CPU to enter a sleep state in which it consumes much
    // less energy. See: https://en.wikipedia.org/wiki/HLT_(x86_instruction)
    __asm__("hlt");
  }
}
