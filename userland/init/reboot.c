#include "init.h"
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>

void _reboot()
{
  printf("Restarting system now...\n");
  reboot(REBOOT_CMD_RESTART);
}
