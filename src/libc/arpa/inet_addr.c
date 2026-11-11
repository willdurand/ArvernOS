#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>

in_addr_t inet_addr(const char* s)
{
  if (s == NULL) {
    return -1;
  }

  uint8_t ip[4] = { 0 };

  for (uint8_t i = 0; i < 4; i++) {
    int value = 0;

    bool has_digits = false;
    while (*s >= '0' && *s <= '9') {
      has_digits = true;

      value = value * 10 + (*s - '0');
      if (value > 255) {
        return -1;
      }

      s++;
    }

    if (!has_digits) {
      return -1;
    }

    ip[i] = (uint8_t)value;

    if (i < 3) {
      if (*s != '.') {
        return -1;
      }

      s++;
    } else if (*s != '\0') {
      return -1;
    }
  }

  return inet_addr2(ip);
}

in_addr_t inet_addr2(uint8_t ip[4])
{
  uint32_t host_addr = ((uint32_t)ip[0] << 24) | ((uint32_t)ip[1] << 16) |
                       ((uint32_t)ip[2] << 8) | ((uint32_t)ip[3]);

  return htonl(host_addr);
}
