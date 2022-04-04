#include <fs/proc.h>

#include <initcall.h>
#include <osinfo.h>
#include <stdio.h>

int proc_read_version(char* buf, size_t size)
{
  snprintf(buf,
           size,
           "%s %s (%s) %s\n",
           KERNEL_NAME,
           KERNEL_VERSION,
           KERNEL_GIT_HASH,
           KERNEL_TARGET);

  return 0;
}

int proc_version_init()
{
  proc_fs_register_readonly_file("version", &proc_read_version);

  return 0;
}

initcall_fs_register(proc_version_init);
