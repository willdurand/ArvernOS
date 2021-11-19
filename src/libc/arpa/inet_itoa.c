#include <arpa/inet.h>
#include <arvern/utils.h>
#include <stdio.h>

void inet_itoa(uint8_t ip[4], char* buf, size_t len)
{
  UNUSED(len);

  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}
