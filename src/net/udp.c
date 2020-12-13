#include "udp.h"
#include <arpa/inet.h>
#include <core/debug.h>
#include <net/dhcp.h>
#include <net/dns.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <string.h>

void udp_receive_packet(net_interface_t* interface,
                        uint8_t* packet,
                        ipv4_header_t* ipv4_header)
{
  uint8_t* ip_data = packet + (4 * ipv4_header->ihl);

  udp_header_t udp_header = { 0 };
  memcpy(&udp_header, ip_data, sizeof(udp_header_t));
  udp_header.src_port = ntohs(udp_header.src_port);
  udp_header.dst_port = ntohs(udp_header.dst_port);
  udp_header.len = ntohs(udp_header.len);
  udp_header.checksum = ntohs(udp_header.checksum);

  DEBUG("udp packet received: src_port=%d dst_port=%d len=%d checksum=%x",
        udp_header.src_port,
        udp_header.dst_port,
        udp_header.len,
        udp_header.checksum);

  uint8_t* udp_data = ip_data + sizeof(udp_header_t);

  switch (udp_header.dst_port) {
    case PORT_DNS_CLIENT:
      dns_receive_packet(interface, udp_data, &udp_header);
      break;
    case PORT_DHCP_CLIENT:
      dhcp_receive_packet(interface, udp_data, &udp_header);
      break;
    default:
      DEBUG("dropping udp packet (src_port=%d, dst_port=%d) because it cannot "
            "be handled yet",
            udp_header.src_port,
            udp_header.dst_port);
  }
}

void udp_send_packet(net_interface_t* interface,
                     uint16_t src_port,
                     uint8_t dst_mac[6],
                     struct sockaddr_in* dst_addr,
                     uint8_t* data,
                     uint32_t len)
{
  uint16_t udp_len = sizeof(udp_header_t) + len;
  udp_header_t udp_header = { .src_port = htons(src_port),
                              .dst_port = dst_addr->sin_port,
                              .len = htons(udp_len),
                              .checksum = 0 };

  // Compute a checksum. Not very efficient but it seems to work. It is heavily
  // inspired by:
  // https://stackoverflow.com/questions/45908909/my-udp-checksum-calculation-gives-wrong-results-every-time
  // See also:
  // https://en.wikipedia.org/wiki/User_Datagram_Protocol#UDP_datagram_structure
  uint32_t pseudo_header_len = 9 + (len / 2);
  if (len & 1) {
    pseudo_header_len += 1;
  }
  pseudo_header_len *= sizeof(uint16_t);

  uint16_t* pseudo_header = malloc(pseudo_header_len);
  memset(pseudo_header, 0, pseudo_header_len);

  pseudo_header[0] = interface->ip[0] | (interface->ip[1] << 8);
  pseudo_header[1] = interface->ip[2] | (interface->ip[3] << 8);
  pseudo_header[2] = (uint16_t)(dst_addr->sin_addr.s_addr >> 16);
  pseudo_header[3] = (uint16_t)(dst_addr->sin_addr.s_addr);
  pseudo_header[4] = htons(IPV4_PROTO_UDP);
  pseudo_header[5] = htons(len + sizeof(udp_header_t));
  pseudo_header[6] = htons(src_port);
  pseudo_header[7] = dst_addr->sin_port;
  pseudo_header[8] = htons(len + sizeof(udp_header_t));

  uint32_t idx = 9;
  for (uint32_t i = 0; i < len; i += 2) {
    if (i + 1 < len) {
      pseudo_header[idx++] = data[i] | (data[i + 1] << 8);
    } else {
      pseudo_header[idx++] = data[i] | (0x0 << 8);
    }
  }

  udp_header.checksum = ipv4_checksum(pseudo_header, pseudo_header_len);

  free(pseudo_header);

  // Create IPv4 datagram, encapsulating the UDP packet and data.
  uint32_t datagram_len = sizeof(udp_header_t) + sizeof(ipv4_header_t) + len;

  ipv4_header_t ipv4_header = ipv4_create_header(interface->ip,
                                                 dst_addr->sin_addr.s_addr,
                                                 IPV4_PROTO_UDP,
                                                 IPV4_FLAG_DF,
                                                 datagram_len);

  uint8_t* datagram = malloc(datagram_len);
  memcpy(datagram, &ipv4_header, sizeof(ipv4_header_t));
  memcpy(datagram + sizeof(ipv4_header_t), &udp_header, sizeof(udp_header_t));
  memcpy(datagram + sizeof(ipv4_header_t) + sizeof(udp_header_t), data, len);

  ethernet_transmit_frame(
    interface, dst_mac, ETHERTYPE_IPV4, datagram, datagram_len);

  free(datagram);
}
