/**
 * @file
 * @see https://wiki.osdev.org/PCI
 *
 * This module manages the Peripheral Component Interconnect (PCI) bus.
 */
#ifndef CORE_PCI_H
#define CORE_PCI_H

#include <stdint.h>

#define pci_deviceICES_PER_BUS 32

// I/O locations
#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_NONE 0xFFFF

#define PCI_VENDOR_ID      0x00
#define PCI_DEVICE_ID      0x02
#define PCI_COMMAND        0x04
#define PCI_CLASS          0X0B
#define PCI_SUBCLASS       0X0A
#define PCI_BASE_ADDR_0    0x10
#define PCI_INTERRUPT_LINE 0x3C

// See: https://wiki.osdev.org/PCI#Configuration_Space_Access_Mechanism_.231
typedef union pci_device
{
  uint32_t packed;
  struct
  {
    uint8_t zeros : 2;
    uint8_t register_offset : 6;
    /// This is to choose a specific function in a device (if the device
    /// supports multiple functions).
    uint8_t function_number : 3;
    /// This is to select the specific device on the PCI bus.
    uint8_t device_number : 5;
    /// This is to allow the configuration software to choose a specific PCI
    /// bus in the system.
    uint8_t bus_number : 8;
    uint8_t reserved : 7;
    /// This is for determining when accesses to CONFIG_DATA should be
    /// translated to configuration cycles
    uint8_t enable : 1;
  };
} pci_device_t;

/**
 * Given a `vendor_id` and `device_id`, this function returns the corresponding
 * PCI device if it exists.
 *
 * @param vendor_id ID of the manufacturer of the device
 * @param device_id ID of the particular device
 * @return the PCI device or a zero'ed structure
 */
pci_device_t pci_get_device(uint16_t vendor_id, uint16_t device_id);

/**
 * Reads a field from the configuration space.
 *
 * @param device the PCI device
 * @param field the field to read
 * @param size the size of the value for the field to read
 * @return the value for the field read
 */
uint32_t pci_read(pci_device_t device, uint32_t field, uint8_t size);

/**
 * Writes a value to a field in the configuration space.
 *
 * @param device the PCI device
 * @param field the field to update
 * @param value the new value for the field
 */
void pci_write(pci_device_t device, uint32_t field, uint32_t value);

#endif
