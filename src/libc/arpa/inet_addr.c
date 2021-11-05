#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

in_addr_t inet_addr(const char* s)
{
  uint8_t ip[4] = { 0 };

  char* p = strtok((char*)s, ".");

  uint8_t i = 0;
  while (p != NULL && i < 4) {
    ip[i++] = atoi(p);
    p = strtok(NULL, ".");
  }

  return inet_addr2(ip);
}

in_addr_t inet_addr2(uint8_t ip[4])
{
  return (in_addr_t)(ip[0] | (ip[1] << 8) | (ip[2] << 16) |
                     ((uint32_t)ip[3] << 24));
}
