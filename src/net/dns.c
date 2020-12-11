#include "dns.h"
#include <arpa/inet.h>
#include <core/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t dns_id = 1;

void dns_request(net_interface_t* interface, char* domain)
{
  // TODO: fixme, we don't have the concept of port/socket yet.
  uint16_t src_port = 42796;

  dns_header_t dns_header = {
    .id = htons(dns_id),
    .qdcount = htons(0x0001),
    .flags = htons(0x0100), // recursion requested
  };

  // Transforms a domain name into a DNS query name, e.g.:
  // foo.example.com -> 3foo7example3com
  uint32_t domain_len = strlen(domain);
  uint32_t data_len = (1            // first count
                       + domain_len // dots will be replaced with counts
                       + 1          // END
                       + 2          // Type
                       + 2          // Class
                       ) *
                      sizeof(uint8_t);
  uint8_t* data = malloc(data_len);

  char* _domain = calloc(1, (domain_len + 1) * sizeof(char));
  strcat(_domain, domain);
  strcat(_domain, ".");

  uint32_t j = 0, pos = 0;
  for (uint32_t i = 0; i < domain_len + 1; i++) {
    if (_domain[i] == '.') {
      // add the count first
      data[j++] = i - pos;

      // copy part
      while (pos < i) {
        data[j++] = _domain[pos++];
      }

      pos++;
    }
  }
  // END
  data[j++] = 0;
  // Type
  data[j++] = (uint8_t)(DNS_TYPE_A << 8);
  data[j++] = (uint8_t)(DNS_TYPE_A);
  // Class
  data[j++] = (uint8_t)(DNS_CLASS_IN << 8);
  data[j++] = (uint8_t)(DNS_CLASS_IN);

  free(_domain);

  uint16_t packet_len = sizeof(dns_header_t) + data_len;
  uint8_t* packet = malloc(packet_len);
  memcpy(packet, &dns_header, sizeof(dns_header_t));
  memcpy(packet + sizeof(dns_header_t), data, data_len);

  free(data);

  DEBUG_OUT("sending dns packet: id=0x%04x flags=0x%04x",
            ntohs(dns_header.id),
            ntohs(dns_header.flags));

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT_DNS),
    .sin_addr = inet_addr2(interface->dns_ip),
  };

  udp_send_packet(
    interface, src_port, interface->dns_mac, &addr, packet, packet_len);

  dns_id++;

  free(packet);
}

void dns_receive_packet(net_interface_t* interface,
                        uint8_t* packet,
                        udp_header_t* header)
{
  dns_header_t dns_header = { 0 };
  memcpy(&dns_header, packet, sizeof(dns_header_t));
  dns_header.id = ntohs(dns_header.id);
  dns_header.flags = ntohs(dns_header.flags);
  dns_header.qdcount = ntohs(dns_header.qdcount);
  dns_header.ancount = ntohs(dns_header.ancount);
  dns_header.nscount = ntohs(dns_header.nscount);
  dns_header.arcount = ntohs(dns_header.arcount);

  DEBUG_OUT("dns packet received: id=0x%04x qdcount=%d ancount=%d",
            dns_header.id,
            dns_header.qdcount,
            dns_header.ancount);

  uint8_t* dns_data = packet + sizeof(dns_header_t);

  // Compute the query payload length so that we can skip it to read the
  // answers directly.
  uint16_t query_len = 0;
  for (uint16_t query = 0; query < dns_header.qdcount; query++) {
    while (dns_data[query_len++] != 0x00) {
      ;
    }
    query_len += 2; // type
    query_len += 2; // class
  }

  if (dns_header.ancount > 0) {
    // The following code only handles the first answer.
    dns_answer_header_t answer_header = { 0 };
    memcpy(&answer_header, dns_data + query_len, sizeof(dns_answer_header_t));
    answer_header.name = ntohs(answer_header.name);
    answer_header.type = ntohs(answer_header.type);
    answer_header.class = ntohs(answer_header.class);
    answer_header.ttl = ntohl(answer_header.ttl);
    answer_header.data_len = ntohs(answer_header.data_len);

    if (answer_header.class == DNS_CLASS_IN && answer_header.data_len == 4) {
      uint8_t addr[4] = { 0 };
      memcpy(addr,
             dns_data + query_len + sizeof(dns_answer_header_t),
             answer_header.data_len);

      // TODO: fixme
      printf("DNS response received: name=0x%04x type=0x%04x class=0x%04x\n  "
             "                     ttl=%lu len=%d address=%d.%d.%d.%d\n",
             answer_header.name,
             answer_header.type,
             answer_header.class,
             answer_header.ttl,
             answer_header.data_len,
             addr[0],
             addr[1],
             addr[2],
             addr[3]);
    } else {
      printf("Unsupported DNS response class received\n");
    }
  } else {
    printf("No response received\n");
  }
}
