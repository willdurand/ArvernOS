#include <kshell/kshell.h>

#include <sys/syscall.h>

void power_off(int cmd)
{
  reboot(cmd);
}
