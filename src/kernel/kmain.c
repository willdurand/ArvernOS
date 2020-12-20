#include "kmain.h"
#include <config/inish.h>
#include <core/elf.h>
#include <core/isr.h>
#include <core/multiboot.h>
#include <core/port.h>
#include <drivers/cmos.h>
#include <drivers/keyboard.h>
#include <drivers/rtl8139.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/video/grub-framebuffer/grub-framebuffer.h>
#include <drivers/video/video_api.h>
#include <fs/debug.h>
#include <fs/proc.h>
#include <fs/sock.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <img/png_loader.h>
#include <kernel/console.h>
#include <kernel/kshell.h>
#include <kernel/panic.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <net/net.h>
#include <resources/embed/will_photo.png.h>
#include <resources/psf1/psf1.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>

void print_welcome_messge();
void print_step(const char* msg);
void print_sub_step(const char* msg);
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
  INFO("%s", msg);
}

void print_sub_step(const char* msg)
{
  printf("  %-72s", msg);
  INFO("%s", msg);
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
  uint64_t tick = timer_tick();

  while (tick == timer_tick()) {
    ;
  }
}

void kmain(uint64_t addr)
{
  multiboot_info_t* mbi = (multiboot_info_t*)addr;

  console_init();

  print_welcome_messge();

  // enable serial ports, COM1 for debug logs, COM2 for console.
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
<<<<<<< HEAD
  serial_init(SERIAL_COM2, SERIAL_SPEED_115200);
  DEBUG_OUT("%s has started", KERNEL_NAME);
=======
  INFO("%s %s (%s) / Built on: %s at %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       GIT_HASH,
       KERNEL_DATE,
       KERNEL_TIME);
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74

  DEBUG_OUT("%s", "initializing interruptions");
  print_step("initializing interruptions");
  isr_init();
  irq_init();
  print_ok();

  DEBUG_OUT("%s", "initializing frame allocator");
  print_step("initializing frame allocator");
  frame_init(mbi);
  print_ok();

  DEBUG_OUT("%s", "initializing paging");
  print_step("initializing paging");
  paging_init(mbi);
  print_ok();

  DEBUG_OUT("%s", "initializing heap allocator");
  print_step("initializing heap allocator");
  alloc_init();
  print_ok();

  DEBUG_OUT("%s", "initializing syscalls");
  print_step("initializing syscalls");
  syscall_init();
  print_ok();

<<<<<<< HEAD
  DEBUG_OUT("%s", "initializing real time clock (cmos)");
  print_step("initializing real time clock (cmos)");
=======
  print_step("initializing cmos (real time clock)");
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74
  cmos_init();
  print_ok();

  DEBUG_OUT("%s", "initializing timer");
  print_step("initializing timer");
  timer_init();
  print_ok();

  print_sub_step("checking interrupts");
  check_interrupts();
<<<<<<< HEAD

  DEBUG_OUT("%s", "initializing keyboard");
  print_step("initializing keyboard");
  keyboard_init();
=======
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74
  print_ok();

  DEBUG_OUT("%s", "initializing virtual file system");
  print_step("initializing virtual file system");
  vfs_init();
  print_ok();

  print_sub_step("mounting tarfs (init ramdisk)");
  multiboot_tag_module_t* module =
    (multiboot_tag_module_t*)find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MODULE);

  inode_t initrd = vfs_mount("/", tar_fs_init((uint64_t)module->mod_start));

  if (initrd) {
    print_ok();

    print_sub_step("mounting debugfs");
    if (vfs_mount(FS_DEBUG_MOUNTPOINT, debug_fs_init())) {
      print_ok();
    } else {
      print_ko();
    }

    print_sub_step("mounting procfs");
    if (vfs_mount(FS_PROC_MOUNTPOINT, proc_fs_init())) {
      print_ok();
    } else {
      print_ko();
    }

    print_sub_step("mounting sockfs");
    if (vfs_mount(FS_SOCK_MOUNTPOINT, sock_fs_init())) {
      print_ok();
    } else {
      print_ko();
    }
  } else {
    print_ko();
  }

  print_step("loading kernel.inish configuration");
  inish_config_t* kernel_cfg = inish_load("/etc/kernel.inish");

  if (kernel_cfg == NULL) {
    print_ko();
  } else {
    print_ok();

    inish_section_t* system = inish_get_section(kernel_cfg, "system");
    char* hostname = inish_get_string(system, "hostname");

    if (hostname != NULL) {
      if (strlen(hostname) > 0) {
<<<<<<< HEAD
        DEBUG_OUT("updating hostname: %s", hostname);
=======
        print_sub_step("setting hostname from config");
        DEBUG("updating hostname: %s", hostname);
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74
        proc_update_hostname(hostname, strlen(hostname));
        print_ok();
      } else {
        DEBUG_OUT("%s", "not updating hostname because value is empty");
      }
    } else {
      DEBUG_OUT("%s", "could not find system/hostname in kernel.inish");
    }
  }

  print_step("initializing network");
  if (rtl8139_init()) {
    uint8_t ip[4] = { 0 };
    uint8_t gateway_ip[4] = { 0 };
    uint8_t dns_ip[4] = { 0 };

    inish_section_t* network = inish_get_section(kernel_cfg, "network");
    opt_bool_t prefer_dhcp = inish_get_bool(network, "prefer-dhcp");
    inish_get_ipv4(network, "ip", ip);
    inish_get_ipv4(network, "gateway_ip", gateway_ip);
    inish_get_ipv4(network, "dns_ip", dns_ip);

    net_interface_init(0,
                       rtl8139_driver(),
                       prefer_dhcp.has_value && prefer_dhcp.value,
                       ip,
                       gateway_ip,
                       dns_ip);
    print_ok();
  } else {
    print_ko();
  }

  if (kernel_cfg != NULL) {
    inish_free(kernel_cfg);
  }

<<<<<<< HEAD
  DEBUG_OUT("%s", "loading kernel system font");
  print_step("loading kernel system font");

  kernel_console_font = psf1_load_font(KERNEL_CONSOLE_FONT_FILENAME);
  kernel_console_font != NULL ? print_ok() : print_ko();

  if (grub_init_framebuffer(mbi) == false) {
    PANIC("Failed to initialize framebuffer")
  }

  grub_framebuffer_set_console_mode();
=======
  // Not needed before so let's initialize it at the end.
  print_step("initializing keyboard");
  keyboard_init();
  print_ok();
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74

  multiboot_tag_string_t* cmdline = (multiboot_tag_string_t*)find_multiboot_tag(
    mbi, MULTIBOOT_TAG_TYPE_CMDLINE);

  if (cmdline && strcmp(cmdline->string, "boot-and-exit") == 0) {
    printf("\n\nboot sequence completed, exiting now...");
    INFO("%s", "boot sequence completed, exiting now...");

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

  /*
    grub_framebuffer_set_canvas_mode();

    video_clear(MAKE_RGBA(0x25, 0x25, 0x25, 0xFF));

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t* pixels = NULL;

    if (!png_load_buffer(
          (const uint8_t*)will_photo_png, &width, &height, &pixels)) {
      video_clear(MAKE_RGBA(0xFF, 0, 0, 0xFF));
    }

    if (pixels != NULL) {
      video_blit(pixels, 0, 0, width, height, width, height, 0, 0);
    }

    while (1) {

      video_swap_buffers();
    }
    */
}
