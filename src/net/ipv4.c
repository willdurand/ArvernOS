#include "ipv4.h"
#include <core/debug.h>
#include <net/ethernet.h>
#include <net/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ipv4_from_value(uint32_t value, uint8_t ip[4]);
void icmpv4_receive_packet(net_interface_t* interface,
                           uint8_t* packet,
                           ipv4_header_t* header);

// TODO: find a better approach to generate such IDs.
static uint16_t ipv4_id = 1;
static uint16_t icmpv4_id = 1;

void ipv4_receive_packet(net_interface_t* interface,
                         uint8_t* data,
                         uint32_t len)
{
  ipv4_header_t header = { 0 };
  memcpy(&header, data, sizeof(ipv4_header_t));

  uint8_t src_ip[4] = { 0 };
  ipv4_from_value(header.src_addr, src_ip);
  DEBUG("received IPv4 packet from: %d.%d.%d.%d on interface=%d",
        src_ip[0],
        src_ip[1],
        src_ip[2],
        src_ip[3],
        interface->id);

  switch (header.proto) {
    case IPV4_PROTO_ICMP:
      icmpv4_receive_packet(interface, data, &header);
      break;
    case IPV4_PROTO_UDP:
      udp_receive_packet(interface, data, &header);
      break;
    default:
      DEBUG("unsupported IP protocol=%02x, dropping packet", header.proto);
  }
}

// From: https://tools.ietf.org/html/rfc1071
uint16_t ipv4_checksum(void* addr, int count)
{
  register uint32_t sum = 0;
  uint16_t* ptr = addr;

  while (count > 1) {
    /*  This is the inner loop */
    sum += *ptr++;
    count -= 2;
  }

  /*  Add left-over byte, if any */
  if (count > 0)
    sum += *(uint8_t*)ptr;

  /*  Fold 32-bit sum to 16 bits */
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  return ~sum;
}

// Retrieve src IP address from the uint32_t value. This might not work
// correctly all the time because of endianness but it works for now...
void ipv4_from_value(uint32_t value, uint8_t ip[4])
{
  memset(ip, 0, 4);
  for (uint8_t i = 0; i < 4; i++) {
    ip[i] = ((uint8_t*)&value)[i];
  }
}

void icmpv4_receive_packet(net_interface_t* interface,
                           uint8_t* data,
                           ipv4_header_t* header)
{
  icmpv4_echo_t icmpv4_echo = { 0 };
  memcpy(&icmpv4_echo, data + (4 * header->ihl), sizeof(icmpv4_echo_t));

  DEBUG("received ICMPv4 packet on interface=%d type=%d code=%d checksum=%x",
        interface->id,
        icmpv4_echo.type,
        icmpv4_echo.code,
        icmpv4_echo.checksum);

  uint8_t src_ip[4] = { 0 };
  ipv4_from_value(header->src_addr, src_ip);

  if (icmpv4_echo.type == ICMPV4_TYPE_REPLY) {
    // TODO: fixme
    printf("PONG from %d.%d.%d.%d icmp_seq=%d ttl=%d\n",
           src_ip[0],
           src_ip[1],
           src_ip[2],
           src_ip[3],
           icmpv4_echo.sequence,
           header->ttl);
  }
}

void ipv4_ping(net_interface_t* interface, uint8_t dst_ip[4])
{
  // Create ICMPv4 ECHO packet.
  icmpv4_echo_t icmpv4_echo = { .type = ICMPV4_TYPE_REQUEST,
                                .code = 0,
                                .checksum = 0,
                                .id = htons(icmpv4_id),
                                .sequence = 0,
                                .data = 0 };
  icmpv4_echo.checksum = ipv4_checksum(&icmpv4_echo, sizeof(icmpv4_echo_t));
  icmpv4_id++;

  // Create IPv4 datagram, encapsulating the ICMPv4 packet.
  uint32_t datagram_len = sizeof(icmpv4_echo_t) + sizeof(ipv4_header_t);

  ipv4_header_t ipv4_header = ipv4_create_header(
    interface->ip, inet_addr2(dst_ip), IPV4_PROTO_ICMP, 0, datagram_len);

  uint8_t* datagram = malloc(datagram_len);
  memcpy(datagram, &ipv4_header, sizeof(ipv4_header_t));
  memcpy(datagram + sizeof(ipv4_header_t), &icmpv4_echo, sizeof(icmpv4_echo));

  ethernet_transmit_frame(
    interface, interface->gateway_mac, ETHERTYPE_IPV4, datagram, datagram_len);

  free(datagram);
}

ipv4_header_t ipv4_create_header(uint8_t src_ip[4],
                                 in_addr_t dst_addr,
                                 uint8_t protocol,
                                 uint16_t flags,
                                 uint16_t len)
{
  in_addr_t src_addr = inet_addr2(src_ip);

  ipv4_header_t ipv4_header = { .ihl = 5,
                                .version = IPV4_VERSION,
                                .service = 0,
                                .len = htons(len),
                                .id = htons(ipv4_id),
                                .flags = htons(flags),
                                .ttl = IPV4_DEFAULT_TTL,
                                .proto = protocol,
                                .src_addr = src_addr,
                                .dst_addr = dst_addr };
  ipv4_header.checksum = ipv4_checksum(&ipv4_header, sizeof(ipv4_header_t));

  ipv4_id++;

  return ipv4_header;
}
