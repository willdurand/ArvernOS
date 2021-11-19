#include <sys/k_syscall.h>

#include <net/dns.h>
#include <net/net.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int k_gethostbyname2(const char* name, struct in_addr* in)
{
  net_interface_t* interface = net_get_interface(0);

  if (interface == NULL) {
    return -1;
  }

  uint8_t ip[4] = { 0 };
  int retval = dns_lookup(interface, name, ip);

  if (retval == 0) {
    in->s_addr = inet_addr2(ip);
  }

  return retval;
}
