/**
 * @file
 * @see https://www2.cs.duke.edu/courses/fall16/compsci356/DNS/DNS-primer.pdf
 */
#ifndef NET_DNS_H
#define NET_DNS_H

#include <net/net.h>
#include <net/udp.h>
#include <sys/types.h>

#define DNS_TYPE_A   0x0001
#define DNS_CLASS_IN 0x0001

typedef struct dns_header
{
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
} __attribute__((packed)) dns_header_t;

typedef struct dns_answer_header
{
  uint16_t name;
  uint16_t type;
  uint16_t class;
  uint32_t ttl;
  uint16_t data_len;
} __attribute__((packed)) dns_answer_header_t;

/**
 * Performs a DNS lookup.
 *
 * @param interface the interface that should be used to send the DNS packet
 * @param domain the domain name that is to be looked up
 */
void dns_request(net_interface_t* interface, char* domain);

/**
 * Receives a DNS packet.
 *
 * @param interface the interface on which the DNS packet has been received
 * @param packet the DNS packet
 * @param udp_header the UDP header
 */
void dns_receive_packet(net_interface_t* interface,
                        uint8_t* packet,
                        udp_header_t* udp_header);

#endif
