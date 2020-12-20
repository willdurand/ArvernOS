/**
 * @file
 * @see https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol
 * @see http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm
 */
#ifndef NET_DHCP_H
#define NET_DHCP_H

#include <net/net.h>
#include <net/udp.h>
#include <sys/types.h>

#define DHCP_MAGIC_COOKIE 0x63825363

#define DHCP_ROUTER       0x03
#define DHCP_DNS          0x06
#define DHCP_REQUESTED_IP 0x32
#define DHCP_MESSAGE_TYPE 0x35
#define DHCP_DHCP_SERVER  0x36

#define DHCP_DISCOVER 0x01
#define DHCP_OFFER    0x02
#define DHCP_REQUEST  0x03
#define DHCP_ACK      0x05

typedef struct dhcp_header
{
  uint8_t opcode;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t xid;
  uint16_t secs;
  uint16_t flags;
  uint32_t ciaddr;
  uint32_t yiaddr;
  uint32_t siaddr;
  uint32_t giaddr;
  uint8_t chaddr[16];
  uint8_t zero[192];
  uint32_t magic_cookie;
  // TODO: it could be larger but then there is a problem with the ethernet
  // and/or physical layers. The packet is transmitted correctly but the next
  // received packet seems corrupted.
  uint8_t options[1024];
} __attribute__((packed)) dhcp_header_t;

/**
 * Configures a network interface using DHCP.
 *
 * DHCP operations fall into four phases: server discovery, IP lease offer, IP
 * lease request, and IP lease acknowledgement. This function implements the
 * four phases and configures the given interface once the IP lease
 * acknowledgement is received.
 *
 * @param interface the network interface to use
 * @return `true` when the DHCP negotiation has succeeded, `false` otherwise
 */
bool dhcp_negotiate(net_interface_t* interface);

/**
 * Receives a DHCP packet.
 *
 * @param interface the interface on which the DHCP packet has been received
 * @param packet the DHCP packet
 * @param header the UDP header
 */
void dhcp_receive_packet(net_interface_t* interface,
                         uint8_t* packet,
                         udp_header_t* header);

#endif
