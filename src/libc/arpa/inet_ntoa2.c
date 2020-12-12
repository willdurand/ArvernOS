#include <arpa/inet.h>
#include <string.h>

// Retrieve IP address from the uint32_t value. This might not work correctly
// all the time because of endianness but it works for now...
void inet_ntoa2(uint32_t value, uint8_t ip[4])
{
  memset(ip, 0, 4);
  for (uint8_t i = 0; i < 4; i++) {
    ip[i] = ((uint8_t*)&value)[i];
  }
}
