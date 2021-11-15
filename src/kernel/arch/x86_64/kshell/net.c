#include "kshell.h"
#include <arpa/inet.h>
#include <net/net.h>
#include <stdio.h>

void net()
{
  uint8_t in_id = 0;
  net_interface_t* in = net_get_interface(in_id);

  if (in == NULL) {
    printf("no interface found\n");
    return;
  }

  char buf[16];

  printf("eth%d:\n", in_id);
  printf("  driver : %s\n", in->driver->get_name());
  inet_itoa(in->ip, buf, 16);
  printf("  ip     : %s\n", buf);
  printf("  mac    : %02x:%02x:%02x:%02x:%02x:%02x\n",
         in->mac[0],
         in->mac[1],
         in->mac[2],
         in->mac[3],
         in->mac[4],
         in->mac[5]);
  inet_itoa(in->gateway_ip, buf, 16);
  printf("  gateway: %s\n", buf);
  inet_itoa(in->dns_ip, buf, 16);
  printf("  dns    : %s\n", buf);
}
