#include "rtl8139.h"
#include <arvern/utils.h>
#include <core/interrupt.h>
#include <core/isr.h>
#include <core/pci.h>
#include <core/port.h>
#include <mmu/paging.h>
// We use the net logger because this driver is for a network card.
#include <net/logging.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const char* get_name();
uint8_t* rtl8139_get_mac_address();
void rtl8139_transmit_frame(void* data, uint32_t len);
void rtl8139_receive_frame();
static void rtl8139_callback(isr_stack_t* stack);

static pci_device_t device = { 0 };
static uint32_t ioaddr = 0;

static uint8_t* tx_buffer = NULL;
static uint64_t tx_buffer_addr = 0;
static uint8_t* rx_buffer = NULL;
static uint32_t rx_index = 0;

static uint8_t transmit_start_registers[4] = { 0x20, 0x24, 0x28, 0x2C };
static uint8_t transmit_command_registers[4] = { 0x10, 0x14, 0x18, 0x1C };
static uint8_t current_transmit_pair = 0;

static const char* driver_name = "RealTek RTL8139";
static uint8_t mac_address[6] = { 0 };
static net_driver_t driver = {
  .get_name = get_name,
  .get_mac_address = rtl8139_get_mac_address,
  .transmit_frame = rtl8139_transmit_frame,
  .receive_frame = NULL, // will be set in `net_interface_init()`.
  .interface = NULL,     // will be set in `net_interface_init()`.
};

bool rtl8139_init()
{
  INFO("initializing %s", driver_name);

  device = pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID);

  if (device.packed == 0) {
    NET_DEBUG("%s", "PCI device not found");
    return false;
  }

  uint32_t ret = pci_read(device, PCI_BASE_ADDR_0, 4);
  ioaddr = ret & (~0x3);

  // Enable PCI Bus Mastering
  uint16_t pci_command_reg = pci_read(device, PCI_COMMAND, 2);
  if (!(pci_command_reg & (1 << 2))) {
    pci_command_reg |= (1 << 2);
    pci_write(device, PCI_COMMAND, pci_command_reg);
  }

  // Set the LWAKE + LWPTN to active high in order to power on the device.
  port_byte_out(ioaddr + RTL8139_REGISTER_CONFIG_1, 0x0);

  // Software reset to clear the RX and TX buffers and set everything back to
  // defaults. (0x10 = reset)
  port_byte_out(ioaddr + RTL8139_REGISTER_COMMAND, 0x10);
  while ((port_byte_in(ioaddr + RTL8139_REGISTER_COMMAND) & 0x10) != 0) {
    ;
  }

  // Initialize transmit buffer.
  tx_buffer = calloc(1, TX_BUFFER_SIZE);
  tx_buffer_addr = translate((uint64_t)tx_buffer).value;

  // Initialize receive buffer.
  rx_buffer = calloc(1, RX_BUFFER_SIZE);
  port_dword_out(ioaddr + RTL8139_REGISTER_RBSTART,
                 translate((uint64_t)rx_buffer).value);

  // Set IMR + ISR.
  // '0x0005' sets the TOK and ROK bits high.
  port_word_out(ioaddr + RTL8139_REGISTER_IMR, 0x0005);

  // Configure receive buffer (RCR).
  // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
  port_dword_out(ioaddr + 0x44, 0xf | (1 << 7));

  // Accept and transmit packets.
  port_byte_out(ioaddr + RTL8139_REGISTER_COMMAND, 0x0C);

  // Register an interrupt handler.
  uint8_t irq_num = pci_read(device, PCI_INTERRUPT_LINE, 1);
  isr_register_handler(32 + irq_num, rtl8139_callback);
  NET_DEBUG("registered interrupt handler: %d", 32 + irq_num);

  // Read MAC address.
  uint32_t mac_part1 = port_dword_in(ioaddr + RTL8139_REGISTER_MAC1);
  uint16_t mac_part2 = port_word_in(ioaddr + RTL8139_REGISTER_MAC2);

  mac_address[0] = mac_part1 >> 0;
  mac_address[1] = mac_part1 >> 8;
  mac_address[2] = mac_part1 >> 16;
  mac_address[3] = mac_part1 >> 24;
  mac_address[4] = mac_part2 >> 0;
  mac_address[5] = mac_part2 >> 8;

  INFO("MAC address is: %02x:%02x:%02x:%02x:%02x:%02x",
       mac_address[0],
       mac_address[1],
       mac_address[2],
       mac_address[3],
       mac_address[4],
       mac_address[5]);

  return true;
}

net_driver_t* rtl8139_driver()
{
  return &driver;
}

uint8_t* rtl8139_get_mac_address()
{
  return mac_address;
}

void rtl8139_transmit_frame(void* data, uint32_t len)
{
  memcpy(tx_buffer, data, len);

  NET_DEBUG("current_transmit_pair=%d tx_buffer_addr=%p len=%d",
            current_transmit_pair,
            tx_buffer_addr,
            len);

  port_dword_out(ioaddr + transmit_start_registers[current_transmit_pair],
                 tx_buffer_addr);
  port_dword_out(ioaddr + transmit_command_registers[current_transmit_pair],
                 len);

  current_transmit_pair++;

  if (current_transmit_pair > 3) {
    current_transmit_pair = 0;
  }
}

void rtl8139_receive_frame()
{
  uint8_t* buffer = rx_buffer;
  uint32_t index = rx_index;

  // 0x01 = buffer is empty
  while ((port_byte_in(ioaddr + RTL8139_REGISTER_COMMAND) & 0x01) == 0) {
    uint32_t offset = index % RX_BUFFER_SIZE;
    uint32_t len = (buffer[3 + index] << 8) + buffer[2 + index];

    // Important: this `frame` should be free'd in the receiver code.
    uint8_t* frame = malloc(sizeof(uint8_t) * len);
    memcpy(frame, &buffer[offset + 4], len);

    if (driver.receive_frame != NULL && driver.interface != NULL) {
      driver.receive_frame(driver.interface, frame, len);
    } else {
      NET_DEBUG("%s",
                "dropping frame because driver isn't bound to an interface.");
      free(frame);
    }

    index = (index + len + 4 + 3) & ~3;

    port_word_out(ioaddr + RTL8139_REGISTER_RX_PTR, index - 16);
  }

  rx_index = index;
}

static void rtl8139_callback(isr_stack_t* stack)
{
  NET_DEBUG("%s", "interrupt received");

  // The loop is there in case we have more than one "thing" to handle when the
  // interrupt is triggered.
  while (1) {
    uint16_t status = port_word_in(ioaddr + RTL8139_REGISTER_ISR);
    NET_DEBUG("status=%u", status);

    // clear interrupt.
    port_word_out(ioaddr + RTL8139_REGISTER_ISR, status);

    if (status == 0) {
      break;
    }

    if (status & RTL8139_ISR_TOK) {
      NET_DEBUG("%s", "frame transmitted");
    }

    if (status & RTL8139_ISR_ROK) {
      NET_DEBUG("%s", "frame received");
      rtl8139_receive_frame();
    }
  }

  UNUSED(*stack);
}

const char* get_name()
{
  return driver_name;
}
