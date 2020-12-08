#include "net.h"
#include <core/debug.h>
#include <net/arp.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static net_interface_t* interfaces[NET_MAX_INTERFACES];

void net_interface_init(uint8_t interface_id,
                        net_driver_t* driver,
                        uint8_t ip[4],
                        uint8_t gateway_ip[4],
                        uint8_t dns_ip[4])
{
  if (interface_id >= NET_MAX_INTERFACES) {
    DEBUG_OUT("could not initialize interface with id=%d", interface_id);
    return;
  }

  DEBUG_OUT("initializing interface id=%d with driver='%s'",
            interface_id,
            driver->get_name());

  net_interface_t* in = malloc(sizeof(net_interface_t));
  in->id = interface_id;
  in->driver = driver;
  in->driver->receive_frame = ethernet_receive_frame;
  in->driver->interface = in;
  memcpy(in->mac, in->driver->get_mac_address(), 6);
  memcpy(in->ip, ip, 6);
  memcpy(in->gateway_ip, gateway_ip, 6);
  memcpy(in->dns_ip, dns_ip, 6);

  arp_request(in, gateway_ip);
  arp_wait_reply(in->gateway_mac);

  DEBUG_OUT("MAC address for %d.%d.%d.%d (gateway) is: "
            "%02x:%02x:%02x:%02x:%02x:%02x",
            in->gateway_ip[0],
            in->gateway_ip[1],
            in->gateway_ip[2],
            in->gateway_ip[3],
            in->gateway_mac[0],
            in->gateway_mac[1],
            in->gateway_mac[2],
            in->gateway_mac[3],
            in->gateway_mac[4],
            in->gateway_mac[5]);

  arp_request(in, dns_ip);
  arp_wait_reply(in->dns_mac);

  DEBUG_OUT("MAC address for %d.%d.%d.%d (dns) is: "
            "%02x:%02x:%02x:%02x:%02x:%02x",
            in->dns_ip[0],
            in->dns_ip[1],
            in->dns_ip[2],
            in->dns_ip[3],
            in->dns_mac[0],
            in->dns_mac[1],
            in->dns_mac[2],
            in->dns_mac[3],
            in->dns_mac[4],
            in->dns_mac[5]);

  interfaces[interface_id] = in;

  DEBUG_OUT("interface id=%d successfully initialized", in->id);
}

net_interface_t* net_get_interface(uint8_t interface_id)
{
  if (interface_id >= NET_MAX_INTERFACES) {
    return NULL;
  }

  return interfaces[interface_id];
}
