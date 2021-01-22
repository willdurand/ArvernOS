#include "kshell.h"
#include <arpa/inet.h>
#include <net/ipv4.h>
#include <net/net.h>
#include <stdio.h>
#include <stdlib.h>

void ping(int argc, char* argv[])
{
  if (argc != 5) {
    printf("usage: %s w x y z\n", argv[0]);
    return;
  }

  uint8_t ip[4] = {
    (uint8_t)atoi(argv[1]),
    (uint8_t)atoi(argv[2]),
    (uint8_t)atoi(argv[3]),
    (uint8_t)atoi(argv[4]),
  };

  net_interface_t* in = net_get_interface(0);

  char buf[16];
  inet_itoa(ip, buf, 16);

  printf("PING %s\n", buf);
  icmpv4_reply_t reply = { 0 };
  int retval = ipv4_ping(in, ip, &reply);

  switch (retval) {
    case 0:
      inet_itoa(reply.src_ip, buf, 16);
      printf(
        "PONG from %s (ttl=%d sequence=%ld)\n", buf, reply.ttl, reply.sequence);
      break;
    default:
      printf("Ping failed (%d)\n", retval);
  }
}
