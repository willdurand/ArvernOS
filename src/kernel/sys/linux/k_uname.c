#include <sys/k_syscall.h>

#include <osinfo.h>
#include <string.h>
#include <sys/utsname.h>

static struct utsname uname = { .sysname = KERNEL_NAME,
                                .nodename = "",
                                .release = KERNEL_VERSION,
                                .version = KERNEL_GIT_HASH,
                                .machine = KERNEL_TARGET };

int k_uname(struct utsname* buf)
{
  memcpy(buf, &uname, sizeof(struct utsname));

  return 0;
}
