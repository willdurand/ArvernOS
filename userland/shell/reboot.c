#include "shell.h"
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>

void _reboot()
{
#ifdef __is_libc
  printf("Restarting system now...\n");
  reboot(REBOOT_CMD_RESTART);
#else
  printf("error: command unsupported on this system\n");
#endif
}
