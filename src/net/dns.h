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

#define DNS_ERR_SOCK      1
#define DNS_ERR_SEND      2
#define DNS_ERR_RECV      3
#define DNS_ERR_CLASS     4
#define DNS_ERR_NO_ANSWER 5

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
 * @param ip the receiving array for the IPv4 address
 * @return `0` on success, something else on error
 */
int dns_lookup(net_interface_t* interface, const char* domain, uint8_t ip[4]);

#endif
