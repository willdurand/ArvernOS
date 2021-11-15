/**
 * @see https://wiki.osdev.org/Reboot
 * @see http://man7.org/linux/man-pages/man2/reboot.2.html
 */
#include <sys/k_syscall.h>

#include <core/isr.h>
#include <core/port.h>
#include <errno.h>
#include <sys/logging.h>
#include <sys/reboot.h>

#define KEYBOARD_STATUS_PORT 0x64
#define RESET_CPU_COMMAND    0xFE

void restart();
void poweroff();

int k_reboot(int command)
{
  SYS_DEBUG("reboot command=%d", command);

  switch (command) {
    case REBOOT_CMD_RESTART:
      restart();
      break;
    case REBOOT_CMD_POWER_OFF:
      poweroff();
    default:
      return -EINVAL;
  }

  return 0;
}

void restart()
{
  SYS_DEBUG("%s", "restarting system");
  printf("Restarting system now...\n");

  isr_disable_interrupts();

  uint8_t status = 0x02;
  while (status & 0x02) {
    status = port_byte_in(KEYBOARD_STATUS_PORT);
  }

  port_byte_out(KEYBOARD_STATUS_PORT, RESET_CPU_COMMAND);

  while (1) {
    __asm__("hlt");
  }
}

void poweroff()
{
  SYS_DEBUG("%s", "powering off system");
  printf("Powering off system now...\n");

  isr_disable_interrupts();

  // Power-off for QEMU, see: https://wiki.osdev.org/Shutdown
  port_word_out(0x604, 0x2000);
}
