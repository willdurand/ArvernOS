/**
 * @file
 * @see https://wiki.osdev.org/RTL8139
 */
#ifndef DRIVERS_RTL8139_H
#define DRIVERS_RTL8139_H

#include <core/pci.h>
#include <net/net.h>
#include <stdbool.h>

#define RTL8139_VENDOR_ID 0x10ec
#define RTL8139_DEVICE_ID 0x8139

#define RTL8139_REGISTER_RBSTART  0x30
#define RTL8139_REGISTER_COMMAND  0x37
#define RTL8139_REGISTER_RX_PTR   0x38
#define RTL8139_REGISTER_IMR      0x3C
#define RTL8139_REGISTER_ISR      0x3E
#define RTL8139_REGISTER_CONFIG_1 0x52
#define RTL8139_REGISTER_MAC1     0x00
#define RTL8139_REGISTER_MAC2     0x04

// Receive OK.
#define RTL8139_ISR_ROK (1 << 0)
// Transmit OK.
#define RTL8139_ISR_TOK (1 << 2)

#define RX_BUFFER_SIZE (8192 + 16 + 1500)
#define TX_BUFFER_SIZE 1536

/**
 * Initializes the RTL8139 driver.
 *
 * @param the PCI device corresponding to a RTL8139 ethernet card
 * @return true when initialization has succeeded, false otherwise
 */
bool rtl8139_init(pci_device_t device);

/**
 * Returns a network driver.
 */
net_driver_t* rtl8139_driver();

#endif
