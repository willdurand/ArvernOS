#ifndef ARPA_INET_H
#define ARPA_INET_H

#include <sys/socket.h>
#include <sys/types.h>

// TODO: ideally, we'd check the endianness of the system but this project is
// for x86_64 only (i.e. it's little endian and we have to use those macros).
#define HTONS(n) ((((n)&0xFF) << 8) | (((n)&0xFF00) >> 8))
#define NTOHS(n) HTONS(n)
#define HTONL(n)                                                               \
  ((((n)&0x000000FF) << 24) | (((n)&0x0000FF00) << 8) |                        \
   (((n)&0x00FF0000) >> 8) | (((n)&0xFF000000) >> 24))
#define NTOHL(n) HTONL(n)

typedef uint32_t in_addr_t;

struct in_addr
{
  in_addr_t s_addr;
};

struct sockaddr_in
{
  int16_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  unsigned char sin_zero[8];
};

uint16_t htons(uint16_t value);

uint16_t ntohs(uint16_t value);

uint32_t htonl(uint32_t value);

uint32_t ntohl(uint32_t value);

#define htons(n) HTONS(n)
#define ntohs(n) NTOHS(n)
#define htonl(n) HTONL(n)
#define ntohl(n) NTOHL(n)

#endif
