#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>

void main()
{
  printf("Looks like everything is alright!\n");

  reboot(REBOOT_CMD_POWER_OFF);
}
