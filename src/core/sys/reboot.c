#include "reboot.h"
#include <core/isr.h>
#include <core/port.h>
#include <errno.h>
#include <sys/reboot.h>

#define KEYBOARD_STATUS_PORT 0x64
#define RESET_CPU_COMMAND    0xFE

void restart();

int kreboot(int command)
{
  errno = 0;

  switch (command) {
    case REBOOT_CMD_RESTART:
      restart();
    default:
      errno = EINVAL;
      return -1;
  }
}

void restart()
{
  irq_disable();

  uint8_t status = 0x02;
  while (status & 0x02) {
    status = port_byte_in(KEYBOARD_STATUS_PORT);
  }

  port_byte_out(KEYBOARD_STATUS_PORT, RESET_CPU_COMMAND);

  while (1) {
    __asm__("hlt");
  }
}
