/**
 * @file
 * @see https://www.saminiir.com/lets-code-tcp-ip-stack-2-ipv4-icmpv4/
 */
#ifndef NET_ICMPV4_H
#define NET_ICMPV4_H

#include <net/ipv4.h>
#include <net/net.h>
#include <stdint.h>

#define ICMPV4_TYPE_REPLY       0
#define ICMPV4_TYPE_UNREACHABLE 3
#define ICMPV4_TYPE_REQUEST     8

typedef struct icmpv4_echo
{
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t id;
  uint16_t sequence;
  uint8_t* data;
} __attribute__((packed)) icmpv4_echo_t;

typedef struct icmpv4_reply
{
  uint8_t src_ip[4];
  uint16_t sequence;
  uint8_t ttl;
} icmpv4_reply_t;

/**
 * Receives a ICMPv4 packet.
 *
 * @param interface the interface on which the ICMPv4 packet has been received
 * @param packet the ICMPv4 packet
 * @param ipv4_header the IPv4 header
 */
void icmpv4_receive_packet(net_interface_t* interface,
                           uint8_t* packet,
                           ipv4_header_t* header);

/**
 * "Pings" an IPv4 address, i.e. sends an ICMPv4 packet to it and waits for an
 * ICMPv4 reply.
 *
 * @param interface the interface to use to send the ICMPv4 request
 * @param ip the IPv4 address to ping
 * @param reply the ICMPv4 reply
 * @return `0` on success, something else otherwise
 */
int icmpv4_send_packet(net_interface_t* interface,
                       uint8_t ip[4],
                       icmpv4_reply_t* reply);

#endif
