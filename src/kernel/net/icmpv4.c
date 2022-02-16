#include <net/icmpv4.h>

#include <arvern/utils.h>
#include <net/logging.h>
#include <stdlib.h>
#include <string.h>

// TODO: Find a better approach to generate such IDs.
static uint16_t icmpv4_id = 1;
static icmpv4_reply_t* icmpv4_reply = NULL;

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

int icmpv4_send_packet(net_interface_t* interface,
                       uint8_t ip[4],
                       icmpv4_reply_t* reply)
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
