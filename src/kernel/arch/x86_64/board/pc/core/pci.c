#include <core/pci.h>

#include <core/logging.h>
#include <core/port.h>
#include <initcall.h>
#include <stdbool.h>

uint32_t pci_read(pci_device_t device, uint32_t field, uint8_t size)
{
  device.register_offset = (field & 0xFC) >> 2;
  device.enable = 1;

  port_dword_out(PCI_CONFIG_ADDR, device.packed);

  if (size == 4) {
    return port_dword_in(PCI_CONFIG_DATA);
  } else if (size == 2) {
    return port_word_in(PCI_CONFIG_DATA + (field & 2));
  } else if (size == 1) {
    return port_byte_in(PCI_CONFIG_DATA + (field & 3));
  }

  return PCI_NONE;
}

void pci_write(pci_device_t device, uint32_t field, uint32_t value)
{
  device.register_offset = (field & 0xFC) >> 2;
  device.enable = 1;

  port_dword_out(PCI_CONFIG_ADDR, device.packed);
  port_dword_out(PCI_CONFIG_DATA, value);
}

static int pci_init()
{
  INFO("%s", "pci: looking for PCI devices");

  for (uint8_t num = 0; num < PCI_DEVICES_PER_BUS; num++) {
    pci_device_t device = { 0 };
    device.bus_number = 0;
    device.device_number = num;
    device.function_number = 0;

    uint32_t v_id = pci_read(device, PCI_VENDOR_ID, 2);
    if (v_id == PCI_NONE) {
      continue;
    }

    uint32_t d_id = pci_read(device, PCI_DEVICE_ID, 2);
    bool matched = false;

    for (pci_driver_t* drv = (pci_driver_t*)&__pci_drivers_start;
         drv < (pci_driver_t*)&__pci_drivers_end;
         drv++) {
      if (drv->vendor_id == v_id && drv->device_id == d_id) {
        CORE_DEBUG(
          "pci: %s claims vendor_id=%#x device_id=%#x", drv->name, v_id, d_id);
        drv->probe(device);
        matched = true;
        break;
      }
    }

    if (!matched) {
      WARN("pci: no driver for vendor_id=%#x device_id=%#x", v_id, d_id);
    }
  }

  return 0;
}

initcall_late_register(pci_init);
