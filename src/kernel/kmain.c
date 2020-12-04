#include "kmain.h"
#include <core/debug.h>
#include <core/elf.h>
#include <core/isr.h>
#include <core/multiboot.h>
#include <core/port.h>
#include <core/sys/syscall.h>
#include <drivers/cmos.h>
#include <drivers/keyboard.h>
#include <drivers/rtl8139.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/video/grub-framebuffer/grub-framebuffer.h>
#include <fs/debug.h>
#include <fs/proc.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <kernel/console.h>
#include <kernel/kshell.h>
#include <kernel/panic.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <resources/psf1/psf1.h>
#include <net/net.h>
#include <stdio.h>
#include <string.h>

void print_welcome_messge();
void print_step(const char* msg);
void print_ok();
void check_interrupts();

PSF1_font_t* kernel_console_font;

void print_welcome_messge()
{
  printf("\033[1;34m%s\033[0m\n", KERNEL_ASCII);
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
  printf("\033[0;32mOK\033[0m");
  printf("]");
}

void print_ko()
{
  printf("[");
  printf("\033[0;31mFAIL\033[0m");
  printf("]");
}

void check_interrupts()
{
  print_step("checking interrupts");

  uint64_t tick = timer_tick();

  while (tick == timer_tick()) {
    ;
  }

  print_ok();
}

void kmain(uint64_t addr)
{
  multiboot_info_t* mbi = (multiboot_info_t*)addr;

  console_init();

  print_welcome_messge();

  // enable serial port
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
  DEBUG("%s has started", KERNEL_NAME);

  DEBUG("%s", "initializing interruptions");
  print_step("initializing interruptions");
  isr_init();
  irq_init();
  print_ok();

  DEBUG("%s", "initializing frame allocator");
  print_step("initializing frame allocator");
  frame_init(mbi);
  print_ok();

  DEBUG("%s", "initializing paging");
  print_step("initializing paging");
  paging_init(mbi);
  print_ok();

  DEBUG("%s", "initializing heap allocator");
  print_step("initializing heap allocator");
  alloc_init();
  print_ok();

  DEBUG("%s", "initializing syscalls");
  print_step("initializing syscalls");
  syscall_init();
  print_ok();

  DEBUG("%s", "initializing real time clock (cmos)");
  print_step("initializing real time clock (cmos)");
  cmos_init();
  print_ok();

  DEBUG("%s", "initializing timer");
  print_step("initializing timer");
  timer_init();
  print_ok();

  check_interrupts();

  DEBUG("%s", "initializing keyboard");
  print_step("initializing keyboard");
  keyboard_init();
  print_ok();

  DEBUG("%s", "initializing virtual file system");
  print_step("initializing virtual file system");
  vfs_init();
  print_ok();

  DEBUG("%s", "initializing network");
  print_step("initializing network");
  if (rtl8139_init()) {
    // TODO: move these values to a config file (ini or yaml maybe) stored in
    // the init ramdisk until we support DHCP.
    uint8_t ip[4] = { 10, 0, 2, 15 };
    uint8_t gateway_ip[4] = { 10, 0, 2, 2 };
    uint8_t dns_ip[4] = { 10, 0, 2, 3 };

    net_interface_init(0, rtl8139_driver(), ip, gateway_ip, dns_ip);
    print_ok();
  } else {
    print_ko();
  }

  DEBUG("%s", "mounting all file systems");
  print_step("mounting all file systems");
  multiboot_tag_module_t* module =
    (multiboot_tag_module_t*)find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MODULE);

  inode_t initrd = vfs_mount("/", tar_fs_init((uint64_t)module->mod_start));

  if (initrd) {
    vfs_mount(FS_DEBUG_MOUNTPOINT, debug_fs_init());
    vfs_mount(FS_PROC_MOUNTPOINT, proc_fs_init());
    print_ok();
  } else {
    print_ko();
  }

  DEBUG("%s", "loading kernel system font");
  print_step("loading kernel system font");

  kernel_console_font = psf1_load_font(KERNEL_CONSOLE_FONT_FILENAME);
  kernel_console_font != NULL ? print_ok() : print_ko();

  if (grub_init_framebuffer(mbi) == false) {
    PANIC("Failed to initialize framebuffer")
  }

  grub_framebuffer_set_console_mode();

  multiboot_tag_string_t* cmdline = (multiboot_tag_string_t*)find_multiboot_tag(
    mbi, MULTIBOOT_TAG_TYPE_CMDLINE);

  if (cmdline && strcmp(cmdline->string, "boot-and-exit") == 0) {
    printf("\n\nboot sequence completed, exiting now...");
    uint64_t uptime = timer_uptime();

    while (timer_uptime() < uptime + 2) {
      __asm__("hlt");
    }

    irq_disable();
    // Power-off for QEMU, see: https://wiki.osdev.org/Shutdown
    port_word_out(0x604, 0x2000);

    return;
  }

  // kshell
  printf("\n");
  kshell_print_prompt();

  while (1) {
    kshell_run(keyboard_get_scancode());

    grub_framebuffer_swap_buffers();
  }
}
