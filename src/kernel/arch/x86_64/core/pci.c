#include <core/pci.h>

#include <core/logging.h>
#include <core/port.h>

pci_device_t pci_get_device(uint16_t vendor_id, uint16_t device_id)
{
  // TODO: This is the minimal amount of code to retrieve a PCI device when
  // there is only one with the same vendor and device IDs and no function.

  for (uint8_t num = 0; num < PCI_DEVICES_PER_BUS; num++) {
    pci_device_t device = { 0 };
    device.bus_number = 0;
    device.device_number = num;
    device.function_number = 0;

    if (pci_read(device, PCI_VENDOR_ID, 1) == PCI_NONE) {
      continue;
    }

    uint32_t d_id = pci_read(device, PCI_DEVICE_ID, 2);
    uint32_t v_id = pci_read(device, PCI_VENDOR_ID, 2);

    if (device_id == d_id && vendor_id == v_id) {
      CORE_DEBUG("found PCI device with device_id=%#x and vendor_id=%#x",
                 device_id,
                 vendor_id);
      return device;
    }
  }

  return (pci_device_t){ 0 };
}

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
