#include "kmain.h"
#include <config/inish.h>
#include <console/console.h>
#include <core/elf.h>
#include <core/multiboot.h>
#include <core/pci.h>
#include <core/port.h>
#include <core/tss.h>
#include <drivers/keyboard.h>
#include <drivers/rtl8139.h>
#include <drivers/serial.h>
#include <fs/tar.h>
#include <init.h>
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

void load_modules(multiboot_info_t* mbi);
void load_network_config(inish_config_t* kernel_cfg, net_driver_t* driver);
void load_symbols(multiboot_tag_module_t* module, uint64_t size);
void load_system_config(inish_config_t* kernel_cfg);

static uintptr_t initrd_addr = 0;

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
  arch_kernel_load_symbols(module->mod_start, size);
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

int init_network()
{
  pci_device_t device = pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID);

  if (device.packed == 0) {
    WARN("%s", "net: rtl8139: PCI device not found");
    return -1;
  }

  net_driver_t* net_driver = NULL;

  if (rtl8139_init(device)) {
    net_driver = rtl8139_driver();
  }

  if (net_driver == NULL) {
    return -2;
  }

  inish_config_t* kernel_cfg = inish_load("/etc/kernel.inish");

  if (kernel_cfg != NULL) {
    INFO("%s", "net: load /etc/kernel.inish configuration");
  }

  load_network_config(kernel_cfg, net_driver);

  if (kernel_cfg != NULL) {
    inish_free(kernel_cfg);
  }

  return 0;
}

init_register(init_network);

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

  tss_init();
  paging_init();
  alloc_init();
  keyboard_init();

  load_modules(mbi);

  if (console_mode_is_vbe()) {
    INFO("%s", "console: switching to fullscreen mode");

    if (!console_fullscreen()) {
      WARN("%s", "console: failed to switch to fullscreen mode");
    }
  }

  kmain_start(initrd_addr, multiboot_get_cmdline());
}
