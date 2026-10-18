#include <core/port.h>

uint8_t port_byte_in(uint16_t port)
{
  uint8_t result = 0;
  __asm__("inb %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

void port_byte_out(uint16_t port, uint8_t value)
{
  __asm__("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t port_word_in(uint16_t port)
{
  uint16_t result = 0;
  __asm__("inw %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

void port_word_out(uint16_t port, uint16_t value)
{
  __asm__("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t port_dword_in(uint16_t port)
{
  uint32_t result = 0;
  __asm__("inl %%dx, %%eax" : "=a"(result) : "Nd"(port));
  return result;
}

void port_dword_out(uint16_t port, uint32_t value)
{
  __asm__("outl %%eax, %%dx" : : "a"(value), "Nd"(port));
}
