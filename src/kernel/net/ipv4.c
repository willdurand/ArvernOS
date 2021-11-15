#include <net/ipv4.h>

#include <core/utils.h>
#include <net/ethernet.h>
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
void icmpv4_receive_packet(net_interface_t* interface,
                           uint8_t* packet,
                           ipv4_header_t* header);

// TODO: find a better approach to generate such IDs.
static uint16_t ipv4_id = 1;
static uint16_t icmpv4_id = 1;

static icmpv4_reply_t* icmpv4_reply = NULL;

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

void icmpv4_receive_packet(net_interface_t* interface,
                           uint8_t* data,
                           ipv4_header_t* header)
{
  UNUSED(*interface);

  icmpv4_echo_t icmpv4_echo = { 0 };
  memcpy(&icmpv4_echo, data + (4 * header->ihl), sizeof(icmpv4_echo_t));

  NET_DEBUG(
    "received ICMPv4 packet on interface=%d type=%d code=%d checksum=%x",
    interface->id,
    icmpv4_echo.type,
    icmpv4_echo.code,
    icmpv4_echo.checksum);

  if (icmpv4_echo.type == ICMPV4_TYPE_REPLY) {
    icmpv4_reply = malloc(sizeof(icmpv4_reply_t));

    icmpv4_reply->sequence = icmpv4_echo.sequence;
    icmpv4_reply->ttl = header->ttl;
    inet_ntoi(header->src_addr, icmpv4_reply->src_ip, 4);
  }
}

int ipv4_ping(net_interface_t* interface, uint8_t ip[4], icmpv4_reply_t* reply)
{
  // Create ICMPv4 ECHO packet.
  icmpv4_echo_t icmpv4_echo = { .type = ICMPV4_TYPE_REQUEST,
                                .code = 0,
                                .checksum = 0,
                                .id = htons(icmpv4_id),
                                .sequence = 0,
                                .data = 0 };
  icmpv4_echo.checksum = ipv4_checksum(&icmpv4_echo, sizeof(icmpv4_echo_t));

  NET_DEBUG("sending ICMP packet: id=0x%04x", ntohs(icmpv4_echo.id));

  struct sockaddr_in dst_addr = { .sin_addr = { .s_addr = inet_addr2(ip) } };

  ipv4_send_packet(interface,
                   &dst_addr,
                   IPV4_PROTO_ICMP,
                   0,
                   (uint8_t*)&icmpv4_echo,
                   sizeof(icmpv4_echo_t));

  uint64_t elapsed = 0;
  while (icmpv4_reply == NULL && elapsed < 10000) {
    ;
  }

  if (icmpv4_reply == NULL) {
    return -1;
  }

  memcpy(reply, icmpv4_reply, sizeof(icmpv4_reply_t));
  free(icmpv4_reply);

  return 0;
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

  ethernet_transmit_frame(
    interface, interface->gateway_mac, ETHERTYPE_IPV4, packet, packet_len);

  free(packet);
}
