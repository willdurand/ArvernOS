#include <fs/proc.h>

#include <initcall.h>
#include <osinfo.h>
#include <stdio.h>
#include <string.h>

int proc_read_version(char* buf, size_t size, uint64_t offset)
{
  snprintf(buf,
           size,
           "%s %s (%s) %s\n",
           KERNEL_NAME,
           KERNEL_VERSION,
           KERNEL_GIT_HASH,
           KERNEL_TARGET);

  size_t len = strlen(buf);
  if (offset >= len) {
    buf[0] = '\0';
    return 0;
  }

  if (offset > 0) {
    memmove(buf, buf + offset, len - offset + 1);
  }

  return 0;
}

int proc_version_init()
{
  proc_fs_register_readonly_file("version", &proc_read_version);

  return 0;
}

initcall_fs_register(proc_version_init);
