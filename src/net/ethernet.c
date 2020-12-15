#include "ethernet.h"
#include "logging.h"
#include <arpa/inet.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <stdlib.h>
#include <string.h>

void ethernet_receive_frame(net_interface_t* interface,
                            uint8_t* data,
                            uint32_t len)
{
  ethernet_header_t frame_header = { 0 };
  memcpy(&frame_header, data, sizeof(ethernet_header_t));
  frame_header.ethertype = ntohs(frame_header.ethertype);

  NET_DEBUG(
    "received frame from: %02x:%02x:%02x:%02x:%02x:%02x on interface=%d",
    frame_header.src_mac[0],
    frame_header.src_mac[1],
    frame_header.src_mac[2],
    frame_header.src_mac[3],
    frame_header.src_mac[4],
    frame_header.src_mac[5],
    interface->id);

  switch (frame_header.ethertype) {
    case ETHERTYPE_ARP:
      arp_receive_packet(interface,
                         &data[sizeof(ethernet_header_t)],
                         len - sizeof(ethernet_header_t));
      break;
    case ETHERTYPE_IPV4:
      ipv4_receive_packet(interface,
                          &data[sizeof(ethernet_header_t)],
                          len - sizeof(ethernet_header_t));
      break;
    default:
      NET_DEBUG("unsupported ethernet frame: type=0x%04x",
                frame_header.ethertype);
  }

  // This is because `data` gets allocated in the driver code.
  k_free(data);
}

void ethernet_transmit_frame(net_interface_t* interface,
                             uint8_t dst_mac[6],
                             uint16_t ethertype,
                             uint8_t* data,
                             uint32_t len)
{
  ethernet_header_t header = { .ethertype = htons(ethertype) };
  memcpy(header.src_mac, interface->mac, 6);
  memcpy(header.dst_mac, dst_mac, 6);

  uint32_t frame_len = sizeof(ethernet_header_t) + len;

  if (frame_len < 64) {
    frame_len = 64;
  }

  uint8_t* frame = (uint8_t*)k_calloc(1, frame_len);
  memcpy(frame, &header, sizeof(ethernet_header_t));
  memcpy(frame + sizeof(ethernet_header_t), data, len);

  interface->driver->transmit_frame(frame, frame_len);

  k_free(frame);
}
