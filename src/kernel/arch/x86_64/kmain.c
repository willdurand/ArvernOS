#include "kmain.h"
#include <config/inish.h>
#include <console/console.h>
#include <core/elf.h>
#include <core/gdt.h>
#include <core/multiboot.h>
#include <core/port.h>
#include <core/tss.h>
#include <drivers/keyboard.h>
#include <drivers/pit.h>
#include <drivers/rtl8139.h>
#include <drivers/serial.h>
#include <fs/tar.h>
#include <kmain.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <net/net.h>
#include <osinfo.h>
#include <panic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/k_syscall.h>

void busywait(uint64_t seconds);
void load_modules(multiboot_info_t* mbi);
void load_network_config(inish_config_t* kernel_cfg, net_driver_t* driver);
void load_symbols(multiboot_tag_module_t* module, uint64_t size);
void load_system_config(inish_config_t* kernel_cfg);
void print_debug_gdt();
void print_debug_tss();

static uintptr_t initrd_addr = 0;

void print_debug_tss()
{
  // From `src/arch/x86_64/boot.asm`.
  extern tss_t tss;

  DEBUG(
    "tss: rsp0=0x%02x rsp1=0x%02x rsp2=0x%02x", tss.rsp0, tss.rsp1, tss.rsp2);
}

void print_debug_gdt()
{
  // From `src/arch/x86_64/boot.asm`.
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
  uint64_t uptime = pit_uptime();
  while (pit_uptime() < (uptime + seconds)) {
    __asm__("hlt");
  }
}

void load_modules(multiboot_info_t* mbi)
{
  for (multiboot_tag_t* tag = (multiboot_tag_t*)mbi->tags;
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (multiboot_tag_t*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
    if (tag->type == MULTIBOOT_TAG_TYPE_MODULE) {
      multiboot_tag_module_t* module = (multiboot_tag_module_t*)tag;
      uint64_t size = module->mod_end - module->mod_start;

      if (strcmp(module->cmdline, "initrd") == 0) {
        initrd_addr = (uintptr_t)module->mod_start;
      } else if (strcmp(module->cmdline, "symbols") == 0) {
        load_symbols(module, size);
      }
    }
  }
}

void load_symbols(multiboot_tag_module_t* module, uint64_t size)
{
  print_step("loading debug symbols");
  arch_kernel_load_symbols(module->mod_start, size);
  print_ok();
}

void load_network_config(inish_config_t* kernel_cfg, net_driver_t* driver)
{
  uint8_t ip[4] = { 0 };
  uint8_t gateway_ip[4] = { 0 };
  uint8_t dns_ip[4] = { 0 };

  inish_section_t* network = inish_get_section(kernel_cfg, "network");
  opt_bool_t prefer_dhcp = inish_get_bool(network, "prefer-dhcp");
  inish_get_ipv4(network, "ip", ip);
  inish_get_ipv4(network, "gateway_ip", gateway_ip);
  inish_get_ipv4(network, "dns_ip", dns_ip);

  net_interface_init(0,
                     driver,
                     // Use DHCP when there is no network config.
                     prefer_dhcp.has_value ? prefer_dhcp.value : true,
                     ip,
                     gateway_ip,
                     dns_ip);
}

void kmain(uintptr_t addr)
{
  multiboot_info_t* mbi = (multiboot_info_t*)addr;
  multiboot_init(mbi);

  // enable serial port
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);

  // This is required to be able to identity map the framebuffer.
  frame_init();

  multiboot_tag_framebuffer_t* entry = multiboot_get_framebuffer();
  console_init(&entry->common);

  kmain_early_start();

  print_step("initializing TSS");
  tss_init();
  print_debug_gdt();
  print_debug_tss();
  print_ok();

  print_step("initializing paging");
  paging_init();
  print_ok();

  print_step("initializing heap allocator");
  alloc_init();
  print_ok();

  print_step("initializing syscalls");
  syscall_init();
  print_ok();

  load_modules(mbi);

  kmain_init_fs(initrd_addr);

  print_step("loading kernel.inish configuration");
  inish_config_t* kernel_cfg = inish_load("/etc/kernel.inish");

  if (kernel_cfg == NULL) {
    print_ko();
  } else {
    print_ok();
  }

  print_step("initializing network");
  if (rtl8139_init()) {
    net_driver_t* rtl8139 = rtl8139_driver();
    load_network_config(kernel_cfg, rtl8139);
    print_ok();
  } else {
    print_ko();
  }

  if (kernel_cfg != NULL) {
    inish_free(kernel_cfg);
  }

  if (console_mode_is_vbe()) {
    print_step("switching to fullscreen mode");
    busywait(1);

    if (!console_fullscreen()) {
      print_ko();
    }
  }

  // Not needed before so let's initialize it at the end.
  print_step("initializing keyboard");
  keyboard_init();
  print_ok();

  kmain_start(multiboot_get_cmdline());
}
