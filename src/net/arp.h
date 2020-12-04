/**
 * @file
 * @see https://tools.ietf.org/html/rfc826
 * @see https://www.saminiir.com/lets-code-tcp-ip-stack-1-ethernet-arp/
 *
 * The Address Resolution Protocol (ARP) is used for dynamically mapping a
 * local MAC address (Ethernet) to a protocol address (IPv4) by broadcasting a
 * packet to all the machines on the LAN. If one of the machines uses this
 * address, it should send a ARP reply.
 *
 * This module allows to send a ARP request on a given interface. The current
 * "wait for a response" implementation is not bound to an interface, though,
 * and it is implemented as a busyloop. There is no ARP cache either. Apart
 * from that, it works great!
 */
#ifndef NET_ARP_H
#define NET_ARP_H

#include <net/net.h>
#include <stdint.h>

#define ARP_HTYPE_ETHERNET 0x0001

#define ARP_REQUEST 0x0001
#define ARP_REPLY   0x0002

typedef struct arp_packet
{
  uint16_t hardware_type;
  uint16_t protocol_type;
  uint8_t hardware_size;
  uint8_t protocol_size;
  uint16_t opcode;
  uint8_t src_mac[6];
  uint8_t src_ip[4];
  uint8_t dst_mac[6];
  uint8_t dst_ip[4];
} __attribute__((packed)) arp_packet_t;

/**
 * ARP request.
 *
 * @param interface the interface to use to send the ARP request
 * @param ip an IPv4 address
 */
void arp_request(net_interface_t* interface, uint8_t ip[4]);

/**
 * Wait a bit to hopefully get a ARP reply. If a ARP reply is received, the
 * buffer will be updated with the MAC address. Otherwise, the buffer will
 * contain 0 values only.
 *
 * @param dst_mac a pointer to a buffer large enough to receive a MAC address
 */
void arp_wait_reply(uint8_t* dst_mac);

/**
 * Handles ARP packets that are received on an interface.
 *
 * @param interface the interface that received the ARP packet
 * @param data the data containing the ARP packet
 * @param len the length of the data
 */
void arp_receive_packet(net_interface_t* interface,
                        uint8_t* data,
                        uint32_t len);

#endif
