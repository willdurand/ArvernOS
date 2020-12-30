#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>

int main()
{
  printf("Looks like everything is alright!\n");

#ifdef __willos__
  reboot(REBOOT_CMD_POWER_OFF);
#endif

  return 0;
}
