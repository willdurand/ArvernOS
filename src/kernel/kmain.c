#include "kmain.h"
#include <config/inish.h>
#include <core/elf.h>
#include <core/gdt.h>
#include <core/isr.h>
#include <core/multiboot.h>
#include <core/port.h>
#include <core/tss.h>
#include <drivers/cmos.h>
#include <drivers/keyboard.h>
#include <drivers/rtl8139.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <fs/debug.h>
#include <fs/proc.h>
#include <fs/sock.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <kernel/console.h>
#include <kernel/kshell.h>
#include <kernel/panic.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <net/net.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>

void print_welcome_message();
void print_step(const char* msg);
void print_sub_step(const char* msg);
void print_ok();
void check_interrupts();
void busywait(uint64_t seconds);
void print_debug_gdt();
void print_debug_tss();

void print_debug_tss()
{
  // From `src/asm/boot.asm`.
  extern tss_t tss;

  DEBUG(
    "tss: rsp0=0x%02x rsp1=0x%02x rsp2=0x%02x", tss.rsp0, tss.rsp1, tss.rsp2);
}

void print_debug_gdt()
{
  // From `src/asm/boot.asm`.
  extern gdt_table_t gdt64;

  DEBUG("gdt64.kernel_code: type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.kernel_code.type,
        gdt64.kernel_code.limit19_16_and_flags);
  DEBUG("gdt64.kernel_data: type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.kernel_data.type,
        gdt64.kernel_data.limit19_16_and_flags);
  DEBUG("gdt64.user_code  : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.user_code.type,
        gdt64.user_code.limit19_16_and_flags);
  DEBUG("gdt64.user_data  : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.user_data.type,
        gdt64.user_data.limit19_16_and_flags);
  DEBUG("gdt64.tss_low    : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.tss_low.type,
        gdt64.tss_low.limit19_16_and_flags);
  DEBUG("gdt64.tss_high   : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.tss_high.type,
        gdt64.tss_high.limit19_16_and_flags);
}

void busywait(uint64_t seconds)
{
  uint64_t uptime = timer_uptime();
  while (timer_uptime() < (uptime + seconds)) {
    __asm__("hlt");
  }
}

void print_welcome_message()
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
  printf("kernel: %-66s", msg);
  INFO("%s", msg);
}

void print_sub_step(const char* msg)
{
  printf("kernel:   %-64s", msg);
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

  // enable serial port
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
  INFO("%s %s (%s) / Built on: %s at %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       GIT_HASH,
       KERNEL_DATE,
       KERNEL_TIME);

  // This is required to be able to identity map the framebuffer.
  frame_init(mbi);

  multiboot_tag_framebuffer_t* entry =
    (multiboot_tag_framebuffer_t*)find_multiboot_tag(
      mbi, MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
  console_init(&entry->common);

  print_welcome_message();
  print_debug_gdt();
  print_debug_tss();

  print_step("initializing interrupt service routine");
  isr_init();
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

  print_step("initializing cmos (real time clock)");
  cmos_init();
  print_ok();

  print_step("initializing timer");
  timer_init();
  print_ok();

  print_sub_step("checking interrupts");
  check_interrupts();
  print_ok();

  print_step("initializing virtual file system");
  vfs_init();
  print_ok();

  print_sub_step("mounting tarfs (init ramdisk)");
  multiboot_tag_module_t* module =
    (multiboot_tag_module_t*)find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MODULE);
  inode_t initrd = vfs_mount("/", tar_fs_init((uint64_t)module->mod_start));

  if (initrd) {
    print_ok();

    print_sub_step("creating /dev virtual directory");
    if (vfs_mount("/dev", vfs_make_directory("dev"))) {
      print_ok();
    } else {
      print_ko();
    }

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
        print_sub_step("setting hostname from config");
        DEBUG("updating hostname: %s", hostname);
        proc_update_hostname(hostname, strlen(hostname));
        print_ok();
      } else {
        DEBUG("%s", "not updating hostname because value is empty");
      }
    } else {
      DEBUG("%s", "could not find system/hostname in kernel.inish");
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

  // Not needed before so let's initialize it at the end.
  print_step("initializing keyboard");
  keyboard_init();
  print_ok();

  if (console_mode_is_vbe()) {
    print_step("switching to fullscreen mode");
    busywait(2);

    if (!console_fullscreen()) {
      print_ko();
      printf("\n");
    }
  } else {
    printf("\n");
  }

  multiboot_tag_string_t* cmdline = (multiboot_tag_string_t*)find_multiboot_tag(
    mbi, MULTIBOOT_TAG_TYPE_CMDLINE);

  if (cmdline && strcmp(cmdline->string, "kshell") == 0) {
    printf("loading kshell...\n");

    kshell_print_prompt();

    while (1) {
      kshell_run(keyboard_get_scancode());
      // This allows the CPU to enter a sleep state in which it consumes much
      // less energy. See: https://en.wikipedia.org/wiki/HLT_(x86_instruction)
      __asm__("hlt");
    }
  }

  int argc = 1;
  char* _cmdline = strdup(cmdline->string);
  strtok(_cmdline, " ");
  while (strtok(NULL, " ") != NULL) {
    argc++;
  }
  free(_cmdline);

  char** argv = (char**)malloc(sizeof(char*) * (argc + 1));
  argv[0] = strtok(cmdline->string, " ");

  for (int i = 1; i < argc; i++) {
    argv[i] = strtok(NULL, " ");
  }
  argv[argc] = NULL;

  printf("kernel: switching to usermode... (%s)\n", argv[0]);
  k_execv(argv[0], argv);

  PANIC("unexpectedly reached end of kmain");
}
