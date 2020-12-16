#include <arpa/inet.h>

void inet_ntoa2(struct in_addr in, char* buf, size_t len)
{
  uint8_t ip[4];
  inet_ntoi(in.s_addr, ip, 4);
  inet_itoa(ip, buf, len);
}
