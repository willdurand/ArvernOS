#include "shell.h"
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>

void _reboot()
{
#ifdef __willos__
  printf("Restarting system now...\n");
  reboot(REBOOT_CMD_RESTART);
#else
  printf("error: command not supported on this system\n");
#endif
}
