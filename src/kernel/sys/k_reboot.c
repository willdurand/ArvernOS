/**
 * @see https://wiki.osdev.org/Reboot
 * @see http://man7.org/linux/man-pages/man2/reboot.2.html
 */
#include <sys/k_syscall.h>

#include <arch/kernel.h>
#include <errno.h>
#include <stdio.h>
#include <sys/logging.h>
#include <sys/reboot.h>

void restart();
void poweroff(int exit_code);

int k_reboot(int command)
{
  SYS_DEBUG("reboot command=%d", command);

  switch (command) {
    case REBOOT_CMD_RESTART:
      restart();
      break;
    case REBOOT_CMD_POWER_OFF:
    case REBOOT_CMD_POWER_OFF_EXIT_CODE_0:
      poweroff(0);
      break;
    case REBOOT_CMD_POWER_OFF_EXIT_CODE_1:
      poweroff(1);
      break;
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

void poweroff(int exit_code)
{
  INFO("powering off system: exit_code=%d", exit_code);
  printf("Powering off system now...\n");

  arch_poweroff(exit_code);
}
