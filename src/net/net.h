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
  net_interface_t* interface;
  const char* (*get_name)();
  uint8_t* (*get_mac_address)();
  void (*transmit_frame)(void* data, uint32_t len);
  void (*receive_frame)(net_interface_t* interface,
                        uint8_t* data,
                        uint32_t len);
};

struct net_interface
{
  net_driver_t* driver;
  uint8_t id;
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
