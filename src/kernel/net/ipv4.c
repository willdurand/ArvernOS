#include <net/ipv4.h>

#include <arvern/utils.h>
#include <net/ethernet.h>
#include <net/icmpv4.h>
#include <net/logging.h>
#include <net/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ipv4_header_t ipv4_create_header(uint8_t src_ip[4],
                                 in_addr_t dst_addr,
                                 uint8_t protocol,
                                 uint16_t flags,
                                 uint16_t len);

// TODO: Find a better approach to generate such IDs.
static uint16_t ipv4_id = 1;

void ipv4_receive_packet(net_interface_t* interface,
                         uint8_t* data,
                         uint32_t len)
{
  UNUSED(len);

  ipv4_header_t header = { 0 };
  memcpy(&header, data, sizeof(ipv4_header_t));

  uint8_t src_ip[4] = { 0 };
  inet_ntoi(header.src_addr, src_ip, 4);
  NET_DEBUG("received IPv4 packet from: %d.%d.%d.%d on interface=%d",
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
      NET_DEBUG("unsupported IP protocol=%02x, dropping packet", header.proto);
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

void ipv4_send_packet(net_interface_t* interface,
                      struct sockaddr_in* dst_addr,
                      uint8_t protocol,
                      uint16_t flags,
                      uint8_t* data,
                      uint32_t len)
{
  uint32_t packet_len = sizeof(ipv4_header_t) + len;

  ipv4_header_t ipv4_header = ipv4_create_header(
    interface->ip, dst_addr->sin_addr.s_addr, protocol, flags, packet_len);

  uint8_t* packet = malloc(packet_len);
  memcpy(packet, &ipv4_header, sizeof(ipv4_header_t));
  memcpy(packet + sizeof(ipv4_header_t), data, len);

  ethernet_send_frame(
    interface, interface->gateway_mac, ETHERTYPE_IPV4, packet, packet_len);

  free(packet);
}
