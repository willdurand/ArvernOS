#include <kshell/kshell.h>

#include <sys/reboot.h>
#include <sys/syscall.h>

void power_off()
{
  reboot(REBOOT_CMD_POWER_OFF);
}
