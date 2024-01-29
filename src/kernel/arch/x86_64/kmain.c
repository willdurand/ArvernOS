#include "kmain.h"
#include <assert.h>
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
#include <initcall.h>
#include <kmain.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <net/net.h>
#include <osinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_modules();
void load_network_config(inish_config_t* kernel_cfg, net_driver_t* driver);
void load_system_config(inish_config_t* kernel_cfg);

static uintptr_t initrd_addr = 0;

void load_modules()
{
  // TODO: See also comment in `src/kernel/arch/x86_64/core/multiboot.c` about
  // the lack of support for more than one module.
  multiboot_tag_module_t* module = multiboot_find(MULTIBOOT_TAG_TYPE_MODULE);

  if (module != NULL && strcmp(module->cmdline, "initrd") == 0) {
    initrd_addr = (uintptr_t)module->mod_start;
  }
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

int network_init()
{
  pci_device_t device = pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID);

  // It is fine to early return with "success" when we don't find the hardware.
  if (device.packed == 0) {
    WARN("%s", "net: rtl8139: PCI device not found");
    return 0;
  }

  net_driver_t* net_driver = NULL;

  if (rtl8139_init(device)) {
    net_driver = rtl8139_driver();
  }

  if (net_driver == NULL) {
    return -1;
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

initcall_late_register(network_init);

void kmain(uintptr_t addr)
{
  multiboot_info_t* mbi = (multiboot_info_t*)addr;
  assert(mbi != NULL);
  multiboot_init(mbi);

  // enable serial port
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);

  multiboot_tag_framebuffer_t* entry = multiboot_get_framebuffer();
  assert(entry != NULL);
  console_init(&entry->common);

  kmain_early_start();

  tss_init();
  frame_init();
  paging_init();
  alloc_init();
  keyboard_init();

  load_modules();

  if (console_mode_is_vbe()) {
    INFO("%s", "console: switching to fullscreen mode");

    if (!console_fullscreen()) {
      WARN("%s", "console: failed to switch to fullscreen mode");
    }
  }

  kmain_start(initrd_addr, multiboot_get_cmdline());
}
