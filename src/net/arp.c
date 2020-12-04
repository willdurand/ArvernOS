#include "arp.h"
#include <core/debug.h>
#include <net/ethernet.h>
#include <stdbool.h>
#include <string.h>

static uint8_t last_arp_mac_addr[6] = { 0 };
static bool last_arp_mac_addr_set = false;

void arp_request(net_interface_t* interface, uint8_t ip[4])
{
  // broadcast
  uint8_t dst_mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

  // Create ARP packet.
  arp_packet_t arp_packet = {
    .hardware_type = HTONS(ARP_HTYPE_ETHERNET),
    .protocol_type = HTONS(ETHERTYPE_IPV4),
    .hardware_size = 6,
    .protocol_size = 4,
    .opcode = HTONS(ARP_REQUEST),
  };
  memcpy(arp_packet.src_mac, interface->mac, 6);
  memcpy(arp_packet.src_ip, interface->ip, 4);
  memcpy(arp_packet.dst_mac, dst_mac, 6);
  memcpy(arp_packet.dst_ip, ip, 4);

  ethernet_transmit_frame(
    interface, &dst_mac, ETHERTYPE_ARP, &arp_packet, sizeof(arp_packet_t));
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

  DEBUG("%s", "no ARP reply");
}

void arp_receive_packet(net_interface_t* interface, uint8_t* data, uint32_t len)
{
  arp_packet_t packet = { 0 };
  memcpy(&packet, data, sizeof(arp_packet_t));
  packet.hardware_type = NTOHS(packet.hardware_type);
  packet.protocol_type = NTOHS(packet.protocol_type);
  packet.opcode = NTOHS(packet.opcode);

  DEBUG("received ARP packet from: %d.%d.%d.%d "
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
      DEBUG("%s", "packet is a request");
      // TODO: add logic to reply to this request.
      break;
    case ARP_REPLY:
      DEBUG("%s", "packet is a reply");
      memcpy(last_arp_mac_addr, packet.src_mac, 6);
      last_arp_mac_addr_set = true;
      break;
  }
}
