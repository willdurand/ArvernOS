/**
 * @file
 * @see https://en.wikipedia.org/wiki/User_Datagram_Protocol
 */
#ifndef NET_UDP_H
#define NET_UDP_H

#include <arpa/inet.h>
#include <net/ipv4.h>
#include <net/net.h>
#include <sys/types.h>

#define PORT_DNS         53
#define PORT_DHCP_SERVER 67
#define PORT_DHCP_CLIENT 68
#define PORT_NTP         123

typedef struct udp_header
{
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t len;
  uint16_t checksum;
} __attribute__((packed)) udp_header_t;

/**
 * Receives a UDP packet.
 *
 * @param interface the interface on which the UDP packet has been received
 * @param packet the UDP packet
 * @param ipv4_header the IPv4 header
 */
void udp_receive_packet(net_interface_t* interface,
                        uint8_t* packet,
                        ipv4_header_t* ipv4_header);

/**
 * Sends a UDP packet.
 *
 * @param interface the interface that should be used to send the UDP packet
 * @param src_port the source port
 * @param dst_mac the destination MAC address
 * @param dst_addr the destination socket address
 * @param data the UDP data
 * @param len the length of the UDP data
 */
void udp_send_packet(net_interface_t* interface,
                     uint16_t src_port,
                     uint8_t dst_mac[6],
                     struct sockaddr_in* dst_addr,
                     uint8_t* data,
                     uint32_t len);

#endif
