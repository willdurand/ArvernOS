#include "miniuart.h"
#include <core/mmio.h>

void delay(uint64_t cycles);

void miniuart_init()
{
  // enable mini uart
  mmio_write(AUX_ENABLE, 1);
  // enable 8 bit mode
  mmio_write(AUX_MU_LCR, 3);
  // set RTS line to be always high
  mmio_write(AUX_MU_MCR, 0);
  // disable receive and transmit interrupts
  mmio_write(AUX_MU_IER, 0);
  // disable interrupts
  mmio_write(AUX_MU_IIR, 0xC6);
  // set baud rate to 115200
  mmio_write(AUX_MU_BAUD, 270);
  // map UART1 to GPIO pins 14 and 15
  mmio_write(GPFSEL1,
             (mmio_read(GPFSEL1) & ~((7 << 12) | (7 << 15))) | (2 << 12) |
               (2 << 15));
  mmio_write(GPPUD, 0);
  delay(150);
  mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
  delay(150);
  // flush GPIO setup
  mmio_write(GPPUDCLK0, 0);
  // enable transmitter and receiver
  mmio_write(AUX_MU_CNTL, 3);
}

char miniuart_getc()
{
  while (1) {
    if (mmio_read(AUX_MU_LSR) & 0x01) {
      break;
    }

    __asm__ __volatile__("nop");
  }

  char c = (mmio_read(AUX_MU_IO) & 0xFF);

  return c == '\r' ? '\n' : c;
}

void miniuart_putc(char c)
{
  while (1) {
    if (mmio_read(AUX_MU_LSR) & 0x20) {
      break;
    }

    __asm__ __volatile__("nop");
  }

  mmio_write(AUX_MU_IO, c);
}

void miniuart_puts(const char* str)
{
  while (*str) {
    miniuart_putc(*str++);
  }
}

void delay(uint64_t cycles)
{
  while (cycles--) {
    __asm__ __volatile__("nop");
  }
}
