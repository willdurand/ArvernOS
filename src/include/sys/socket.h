#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

#include <sys/types.h>

// Domains
#define AF_INET 2
// Types
#define SOCK_DGRAM 2

typedef uint16_t sa_family_t;

typedef uint32_t socklen_t;

struct sockaddr
{
  sa_family_t sa_family;
  char sa_data[14];
};

#endif
