/**
 * @see https://wiki.osdev.org/Reboot
 * @see http://man7.org/linux/man-pages/man2/reboot.2.html
 */
#include "k_syscall.h"
#include "logging.h"
#include <core/port.h>
#include <errno.h>
#include <sys/reboot.h>

#define KEYBOARD_STATUS_PORT 0x64
#define RESET_CPU_COMMAND    0xFE

void restart();

int k_reboot(int command)
{
  SYS_DEBUG("reboot command=%d", command);

  switch (command) {
    case REBOOT_CMD_RESTART:
      restart();
      break;
    default:
      return -EINVAL;
  }

  return 0;
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
