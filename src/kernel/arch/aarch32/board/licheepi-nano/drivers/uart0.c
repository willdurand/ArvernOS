#include "uart0.h"
#include <board.h>
#include <core/gpio.h>
#include <core/mmio.h>

// According to [1][], "Allwinner F1C100s have a different clock tree than
// ARMv7/v8 Allwinner SoCs, which has only one AHB clock and APB clock".
// [1]:
// https://github.com/Icenowy/sunxi-tools/commit/b5a9c11f2c779478a79da00424435cc3b34ddec6
#define BAUD_115200_F1C100S (0x36)
#define NO_PARITY           (0)
#define ONE_STOP_BIT        (0)
#define DAT_LEN_8_BITS      (3)
#define LC_8_N_1            (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

void uart0_init()
{
  // GPIOE1 -> TXD0
  gpio_configure_function(GPIO_E_CTRL_REG, 1, GPIO_E_UART0);
  // GPIOE0 -> RXD0
  gpio_configure_function(GPIO_E_CTRL_REG, 0, GPIO_E_UART0);

  // Open the clock gate for UART0
  mmio_write(F1C100S_CLK_GATING_REG2,
             mmio_read(F1C100S_CLK_GATING_REG2) | 1 << 20);
  // Deassert UART0 reset
  mmio_write(F1C100S_SOFT_RST_REG2, mmio_read(F1C100S_SOFT_RST_REG2) | 1 << 20);

  // Disable interrupts
  mmio_write(UART0_IER, 0x0);
  // Enable and reset FIFO
  mmio_write(UART0_FCR, 0x07);
  // Disable flow control
  mmio_write(UART0_MCR, 0x0);
  // Divisor Latch Access bit set
  mmio_write(UART0_LCR, mmio_read(UART0_LCR) | (1 << 7));
  // Configure baudrate
  mmio_write(UART0_DLL, BAUD_115200_F1C100S & 0xFF);
  mmio_write(UART0_DLH, (BAUD_115200_F1C100S >> 8) & 0xFF);
  // Divisor Latch Access bit clear
  mmio_write(UART0_LCR, mmio_read(UART0_LCR) & ~(1 << 7));
  // Set line control
  mmio_write(UART0_LCR, (mmio_read(UART0_LCR) & ~0x1F) | LC_8_N_1);
}

void uart0_putc(char c)
{
  while (!(mmio_read(UART0_LSR) & (1 << 6))) {
    __asm__("nop");
  }

  mmio_write(UART0_THR, c);
}

void uart0_puts(const char* str)
{
  while (*str) {
    if (*str == '\n') {
      uart0_putc('\r');
    }

    uart0_putc(*str++);
  }
}

char uart0_getc()
{
  while (!(mmio_read(UART0_LSR) & (1 << 0))) {
    __asm__("nop");
  }

  char c = (mmio_read(UART0_RBR) & 0xFF);

  return c == '\r' ? '\n' : c;
}
