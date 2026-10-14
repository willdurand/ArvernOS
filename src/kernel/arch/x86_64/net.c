#include <config/inish.h>
#include <core/pci.h>
#include <drivers/rtl8139.h>
#include <initcall.h>
#include <logging.h>
#include <net/net.h>
#include <stdint.h>

static void load_network_config(inish_config_t* kernel_cfg,
                                net_driver_t* driver)
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
