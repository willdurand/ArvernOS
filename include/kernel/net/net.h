/**
 * @file
 * @brief This module defines network interfaces and drivers.
 *
 * In this project, a network driver is an abstraction of a physical device,
 * like an Ethernet card for instance. A network driver defines a set of
 * functions to transmit and receive Layer 2 frames.
 *
 * A network interface bridges a network driver and configuration together. The
 * current configuration that is needed for a network interface is:
 *
 * - its MAC address
 * - its IPv4 address
 * - the IPv4 address of the gateway
 * - the IPv4 address of the primary (and only) DNS server
 *
 * Yep, it is very IP oriented but that's how we can show nice things :)
 */
#ifndef NET_NET_H
#define NET_NET_H

#include <stdbool.h>
#include <stdint.h>

#define NET_MAX_INTERFACES 1

typedef struct net_driver net_driver_t;
typedef struct net_interface net_interface_t;

struct net_driver
{
  /// The hardware type (see `ARP_HTYPE_*` values `kernel/net/arp.h`).
  uint16_t type;
  /// The name of the driver.
  const char name[50];
  /// The interface bound to this driver.
  net_interface_t* interface;
  /// This function returns the MAC address of the device.
  uint8_t* (*get_mac_address)();
  /// This function is used to transmit data.
  void (*transmit)(uint8_t* data, uint32_t len);
};

struct net_interface
{
  /// The interface identifier.
  uint8_t id;
  /// The hardware type of the interface.
  uint16_t hardware_type;
  /// The driver bound to this interface.
  net_driver_t* driver;
  /// The callback used by the driver to transfer the received data to the
  /// upper layers.
  void (*receive_frame_callback)(net_interface_t* interface,
                                 uint8_t* data,
                                 uint32_t len);
  // Network configuration.
  uint8_t mac[6];
  uint8_t ip[4];
  uint8_t gateway_mac[6];
  uint8_t gateway_ip[4];
  uint8_t dns_mac[6];
  uint8_t dns_ip[4];
};

/**
 * Initializes a network interface.
 *
 * When `prefer_dhcp` is set to `true`, DHCP is used. If it fails, the static
 * configuration will be used instead.
 *
 * @param interface_id an interface ID
 * @param driver a network driver
 * @param prefer_dhcp whether to use DHCP
 * @param ip a static IPv4 address for the host
 * @param gateway_ip a static IPv4 address for the gateway
 * @param dns_ip a static IPv4 address for the primary DNS server
 */
void net_interface_init(uint8_t interface_id,
                        net_driver_t* driver,
                        bool prefer_dhcp,
                        uint8_t ip[4],
                        uint8_t gateway_ip[4],
                        uint8_t dns_ip[4]);

/**
 * Returns the network interface for the given ID.
 *
 * @param interface_id an interface ID
 * @return the network interface if it exists, `NULL` otherwise
 */
net_interface_t* net_get_interface(uint8_t interface_id);

#endif
