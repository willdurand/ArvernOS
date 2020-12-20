#include <arpa/inet.h>
#include <string.h>

// Retrieve IP address from the uint32_t value. This might not work correctly
// all the time because of endianness but it works for now...
void inet_ntoi(uint32_t value, uint8_t* buf, size_t len)
{
  memset(buf, 0, len);

  size_t ipv4_len = 4;

  if (len >= ipv4_len) {
    for (uint8_t i = 0; i < ipv4_len; i++) {
      buf[i] = ((uint8_t*)&value)[i];
    }
  }
}
