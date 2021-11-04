#include "arp.h"
#include "logging.h"
#include <arpa/inet.h>
#include <core/utils.h>
#include <net/ethernet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void arp_reply(net_interface_t* interface, arp_packet_t* request);

static uint8_t last_arp_mac_addr[6] = { 0 };
static bool last_arp_mac_addr_set = false;

void arp_request(net_interface_t* interface, uint8_t ip[4])
{
  // broadcast
  uint8_t dst_mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

  // Create ARP packet.
  arp_packet_t arp_packet = {
    .hardware_type = htons(ARP_HTYPE_ETHERNET),
    .protocol_type = htons(ETHERTYPE_IPV4),
    .hardware_size = 6,
    .protocol_size = 4,
    .opcode = htons(ARP_REQUEST),
  };
  memcpy(arp_packet.src_mac, interface->mac, 6);
  memcpy(arp_packet.src_ip, interface->ip, 4);
  memcpy(arp_packet.dst_mac, dst_mac, 6);
  memcpy(arp_packet.dst_ip, ip, 4);

  uint8_t packet_len = sizeof(arp_packet_t);
  uint8_t* packet = malloc(packet_len);
  memcpy(packet, &arp_packet, sizeof(arp_packet_t));

  ethernet_transmit_frame(
    interface, dst_mac, ETHERTYPE_ARP, packet, packet_len);

  free(packet);
}

void arp_wait_reply(uint8_t* dst_mac)
{
  memset(dst_mac, 0, 6);

  for (uint32_t i = 0; i < 10000; i++) {
    if (last_arp_mac_addr_set) {
      memcpy(dst_mac, last_arp_mac_addr, 6);
      last_arp_mac_addr_set = false;
      return;
    }
  }

  NET_DEBUG("%s", "no ARP reply");
}

void arp_receive_packet(net_interface_t* interface, uint8_t* data, uint32_t len)
{
  UNUSED(len);

  arp_packet_t packet = { 0 };
  memcpy(&packet, data, sizeof(arp_packet_t));
  packet.hardware_type = ntohs(packet.hardware_type);
  packet.protocol_type = ntohs(packet.protocol_type);
  packet.opcode = ntohs(packet.opcode);

  NET_DEBUG("received ARP packet from: %d.%d.%d.%d "
            "(%02x:%02x:%02x:%02x:%02x:%02x) on interface=%d",
            packet.src_ip[0],
            packet.src_ip[1],
            packet.src_ip[2],
            packet.src_ip[3],
            packet.src_mac[0],
            packet.src_mac[1],
            packet.src_mac[2],
            packet.src_mac[3],
            packet.src_mac[4],
            packet.src_mac[5],
            interface->id);

  switch (packet.opcode) {
    case ARP_REQUEST:
      NET_DEBUG("%s", "packet is a request");

      if (packet.dst_ip[0] == interface->ip[0] &&
          packet.dst_ip[1] == interface->ip[1] &&
          packet.dst_ip[2] == interface->ip[2] &&
          packet.dst_ip[3] == interface->ip[3]) {
        arp_reply(interface, &packet);
      }
      break;
    case ARP_REPLY:
      NET_DEBUG("%s", "packet is a reply");
      memcpy(last_arp_mac_addr, packet.src_mac, 6);
      last_arp_mac_addr_set = true;
      break;
  }
}

void arp_reply(net_interface_t* interface, arp_packet_t* request)
{
  arp_packet_t reply = {
    .hardware_type = htons(request->hardware_type),
    .hardware_size = 6,
    .protocol_type = htons(request->protocol_type),
    .protocol_size = 4,
    .opcode = htons(ARP_REPLY),
  };
  memcpy(reply.src_mac, interface->mac, 6);
  memcpy(reply.src_ip, interface->mac, 4);
  memcpy(reply.dst_mac, request->src_mac, 6);
  memcpy(reply.dst_ip, request->src_ip, 4);

  uint8_t packet_len = sizeof(arp_packet_t);
  uint8_t* packet = malloc(packet_len);
  memcpy(packet, &reply, sizeof(arp_packet_t));

  ethernet_transmit_frame(
    interface, request->src_mac, ETHERTYPE_ARP, packet, packet_len);

  free(packet);
}
