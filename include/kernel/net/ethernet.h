/**
 * @file
 * @see https://en.wikipedia.org/wiki/Ethernet_frame
 * @see https://wiki.osdev.org/Network_Stack
 */
#ifndef NET_ETHERNET_H
#define NET_ETHERNET_H

#include <net/net.h>
#include <stdint.h>

#define ETHERTYPE_ARP  0x0806
#define ETHERTYPE_IPV4 0x0800

typedef struct ethernet_header
{
  uint8_t dst_mac[6];
  uint8_t src_mac[6];
  uint16_t ethertype;
} __attribute__((packed)) ethernet_header_t;

/**
 * Receives an Ethernet frame on an interface. This is a handler that gets
 * called by the lower layer (i.e. the network driver). After having decoded
 * the frame, this handler will likely call a handler of this layer (2) or the
 * layer above (3).
 *
 * @param interface the network interface that received the frame
 * @param data the data containing the Ethernet frame
 * @param len the length of the data
 */
void ethernet_receive_frame(net_interface_t* interface,
                            uint8_t* data,
                            uint32_t len);

/**
 * Sends an Ethernet frame over an interface.
 *
 * We are at the Ethernet level, so we can only communicate with machines in
 * the same LAN and we use MAC addresses for that. The destination MAC address
 * is therefore used by one of the machines in the LAN, usually the DNS server
 * or the "gateway" (but it could be any other machine for LAN exchanges). We
 * will use the MAC address of the gateway when we will send Internet packets
 * for instance.
 *
 * @param interface the network interface used to send the frame
 * @param dst_mac the destination MAC address
 * @param ethertype a value indicating which protocol is encapsulated in the
 * payload of the frame
 * @param data the encapsulated payload
 * @param len the length of the data
 */
void ethernet_send_frame(net_interface_t* interface,
                         uint8_t dst_mac[6],
                         uint16_t ethertype,
                         uint8_t* data,
                         uint32_t len);

#endif
