/**
 * @see https://wiki.osdev.org/Reboot
 * @see http://man7.org/linux/man-pages/man2/reboot.2.html
 */
#include <sys/k_syscall.h>

#include <arch/kernel.h>
#include <errno.h>
#include <sys/logging.h>
#include <sys/reboot.h>

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

  arch_restart();
}

void poweroff()
{
  SYS_DEBUG("%s", "powering off system");
  printf("Powering off system now...\n");

  arch_poweroff();
}
