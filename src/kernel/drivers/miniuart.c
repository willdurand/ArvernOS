#include <drivers/miniuart.h>

#include <core/mmio.h>

// enable transmitter and receiver
#define GPIO_FUNCTION_ALT5 2

// MMIO offsets (based on the GPIO base address) required to enable mini-uart.
#define OFFSET_GPFSEL0     0x00000
#define OFFSET_GPPUD       0x00094
#define OFFSET_GPPUDCLK0   0x00098
#define OFFSET_AUX_ENABLES 0x15004
#define OFFSET_AUX_MU_IO   0x15040
#define OFFSET_AUX_MU_IER  0x15044
#define OFFSET_AUX_MU_IIR  0x15048
#define OFFSET_AUX_MU_LCR  0x1504C
#define OFFSET_AUX_MU_MCR  0x15050
#define OFFSET_AUX_MU_LSR  0x15054
#define OFFSET_AUX_MU_MSR  0x15058
#define OFFSET_AUX_MU_CNTL 0x15060
#define OFFSET_AUX_MU_STAT 0x15064
#define OFFSET_AUX_MU_BAUD 0x15068

void delay(uint64_t cycles);
uint32_t gpio_configure(uint32_t pin,
                        uint32_t value,
                        uint32_t base,
                        uint32_t field_size);

uint32_t base_addr = 0;

void miniuart_init(uint32_t gpio_base_addr)
{
  base_addr = gpio_base_addr;

  gpio_configure(14, GPIO_FUNCTION_ALT5, gpio_base_addr + OFFSET_GPFSEL0, 3);
  gpio_configure(15, GPIO_FUNCTION_ALT5, gpio_base_addr + OFFSET_GPFSEL0, 3);

  // Remove both the pull-up and pull-down states from pins 14 and 15.
  mmio_write(gpio_base_addr + OFFSET_GPPUD, 0);
  delay(150);
  mmio_write(gpio_base_addr + OFFSET_GPPUDCLK0, (1 << 14) | (1 << 15));
  delay(150);
  mmio_write(gpio_base_addr + OFFSET_GPPUDCLK0, 0);

  // enable mini uart
  mmio_write(gpio_base_addr + OFFSET_AUX_ENABLES, 1);
  // disable auto flow control and disable receiver and transmitter
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_CNTL, 0);
  // disable receive and transmit interrupts
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_IER, 0);
  // enable 8 bit mode
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_LCR, 3);
  // set RTS line to be always high
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_MCR, 0);
  // disable interrupts
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_IIR, 0xC6);
  // set baud rate to 115200
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_BAUD, 270);
  // enable transmitter and receiver
  mmio_write(gpio_base_addr + OFFSET_AUX_MU_CNTL, 3);
}

char miniuart_getc()
{
  while (1) {
    if (mmio_read(base_addr + OFFSET_AUX_MU_LSR) & 0x01) {
      break;
    }

    __asm__("nop");
  }

  char c = (mmio_read(base_addr + OFFSET_AUX_MU_IO) & 0xFF);

  return c == '\r' ? '\n' : c;
}

void miniuart_putc(char c)
{
  while (1) {
    if (mmio_read(base_addr + OFFSET_AUX_MU_LSR) & 0x20) {
      break;
    }

    __asm__("nop");
  }

  mmio_write(base_addr + OFFSET_AUX_MU_IO, c);
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
    __asm__("nop");
  }
}

uint32_t gpio_configure(uint32_t pin,
                        uint32_t value,
                        uint32_t base,
                        uint32_t field_size)
{
  // 1 << 0   1
  // 1 << 1   2
  // 1 << 2   4
  // 1 << 3   8
  uint32_t field_mask = (1 << field_size) - 1;

  if (value > field_mask) {
    return 1;
  }

  uint32_t num_fields = 32 / field_size;
  uint32_t selector_id = base + ((pin / num_fields) * 4);
  uint32_t shift = (pin % num_fields) * field_size;

  uint32_t selector_value = mmio_read(selector_id);
  selector_value &= ~(field_mask << shift);
  selector_value |= value << shift;
  mmio_write(selector_id, selector_value);

  return 0;
}
