#include "dhcp.h"
#include <arpa/inet.h>
#include <logging.h>
#include <stdlib.h>
#include <string.h>

void dhcp_discover(net_interface_t* interface);
void dhcp_handle_offer(net_interface_t* interface);
void dhcp_read_option(uint8_t* options,
                      uint8_t code,
                      uint8_t* buf,
                      uint16_t len);

static uint8_t broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
// Limited broadcast.
static uint8_t broadcast_ip[4] = { 255, 255, 255, 255 };

static uint32_t xid = 1;

static dhcp_header_t* dhcp_offer = NULL;

bool dhcp_negotiate(net_interface_t* interface)
{
  dhcp_discover(interface);

  uint64_t elapsed = 0;
  while (dhcp_offer == NULL && elapsed++ < 10000) {
    ;
  }

  if (dhcp_offer == NULL) {
    return false;
  }

  dhcp_handle_offer(interface);
  free(dhcp_offer);

  elapsed = 0;
  while (dhcp_offer == NULL && elapsed++ < 10000) {
    ;
  }

  if (dhcp_offer == NULL) {
    return false;
  }

  dhcp_handle_offer(interface);
  free(dhcp_offer);

  return true;
}

void dhcp_discover(net_interface_t* interface)
{
  xid++;

  dhcp_header_t dhcp_header = {
    .opcode = DHCP_DISCOVER,
    .htype = 0x01, // ethernet
    .hlen = 0x06,
    .hops = 0,
    .xid = htonl(xid),
    .magic_cookie = htonl(DHCP_MAGIC_COOKIE),
    .options = { DHCP_MESSAGE_TYPE, 0x01, DHCP_DISCOVER, 0xFF },
  };
  memcpy(dhcp_header.chaddr, interface->mac, 6);

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT_DHCP_SERVER),
    .sin_addr = { .s_addr = inet_addr2(broadcast_ip) },
  };

  uint16_t packet_len = sizeof(dhcp_header_t);
  uint8_t* packet = malloc(packet_len);
  memcpy(packet, &dhcp_header, sizeof(dhcp_header_t));

  udp_send_packet(
    interface, PORT_DHCP_CLIENT, broadcast_mac, &addr, packet, packet_len);

  free(packet);
}

void dhcp_receive_packet(net_interface_t* interface,
                         uint8_t* packet,
                         udp_header_t* header)
{
  dhcp_header_t dhcp_header = { 0 };
  memcpy(&dhcp_header, packet, sizeof(dhcp_header_t));
  dhcp_header.xid = ntohl(dhcp_header.xid);

  DEBUG("dhcp packet received: opcode=0x%02x xid=0x%08x",
        dhcp_header.opcode,
        dhcp_header.xid);

  if (dhcp_header.xid != xid) {
    DEBUG("%s", "xid mismatch, dropping dhcp packet");
    return;
  }

  switch (dhcp_header.opcode) {
    case DHCP_OFFER:
      dhcp_offer = malloc(sizeof(dhcp_header_t));
      memcpy(dhcp_offer, &dhcp_header, sizeof(dhcp_header_t));
      break;
    default:
      DEBUG("unsupported DHCP opcode=0x%02x", dhcp_header.opcode);
  }
}

void dhcp_handle_offer(net_interface_t* interface)
{
  uint8_t yiaddr[4] = { 0 };
  inet_ntoi(dhcp_offer->yiaddr, yiaddr, 4);

  uint8_t siaddr[4] = { 0 };
  inet_ntoi(dhcp_offer->siaddr, siaddr, 4);

  uint8_t type[1] = { 0 };
  dhcp_read_option(dhcp_offer->options, DHCP_MESSAGE_TYPE, type, 1);

  DEBUG(
    "received DHCP offer: type=0x%02x yiaddr=%d.%d.%d.%d siaddr=%d.%d.%d.%d",
    type[0],
    yiaddr[0],
    yiaddr[1],
    yiaddr[2],
    yiaddr[3],
    siaddr[0],
    siaddr[1],
    siaddr[2],
    siaddr[3]);

  if (type[0] == DHCP_OFFER) {
    dhcp_header_t dhcp_request = {
      .opcode = DHCP_DISCOVER,
      .htype = 0x01, // ethernet
      .hlen = 0x06,
      .hops = 0,
      .xid = htonl(xid),
      .ciaddr = dhcp_offer->yiaddr,
      .siaddr = dhcp_offer->siaddr,
      .magic_cookie = htonl(DHCP_MAGIC_COOKIE),
      .options = { DHCP_MESSAGE_TYPE,
                   0x01,
                   DHCP_REQUEST,
                   DHCP_REQUESTED_IP,
                   0x04,
                   yiaddr[0],
                   yiaddr[1],
                   yiaddr[2],
                   yiaddr[3],
                   DHCP_DHCP_SERVER,
                   0x04,
                   siaddr[0],
                   siaddr[1],
                   siaddr[2],
                   siaddr[3],
                   0xFF },
    };
    memcpy(dhcp_request.chaddr, interface->mac, 6);

    struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(PORT_DHCP_SERVER),
      .sin_addr = { .s_addr = inet_addr2(broadcast_ip) },
    };

    uint16_t packet_len = sizeof(dhcp_header_t);
    uint8_t* packet = malloc(packet_len);
    memcpy(packet, &dhcp_request, sizeof(dhcp_header_t));

    udp_send_packet(
      interface, PORT_DHCP_CLIENT, broadcast_mac, &addr, packet, packet_len);

    free(packet);
  } else if (type[0] == DHCP_ACK) {
    memcpy(interface->ip, yiaddr, 4);

    uint8_t gateway_ip[4] = { 0 };
    dhcp_read_option(dhcp_offer->options, DHCP_ROUTER, gateway_ip, 4);
    memcpy(interface->gateway_ip, gateway_ip, 4);

    uint8_t dns_ip[4] = { 0 };
    dhcp_read_option(dhcp_offer->options, DHCP_DNS, dns_ip, 4);
    memcpy(interface->dns_ip, dns_ip, 4);
  } else {
    DEBUG("%s", "unsupported DHCP message type");
  }
}

void dhcp_read_option(uint8_t* options,
                      uint8_t code,
                      uint8_t* buf,
                      uint16_t len)
{
  for (uint16_t i = 0; i < 1024; i++) {
    if (options[i] == code && options[i + 1] == len) {
      DEBUG("found option with code=0x%02x", code);
      memcpy(buf, options + i + 2, len);
      break;
    }
  }
}
