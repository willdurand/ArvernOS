#include <fs/proc.h>

#include <initcall.h>
#include <osinfo.h>
#include <stdio.h>
#include <string.h>

int proc_read_version(char* buf, size_t size, uint64_t offset)
{
  char content[256];

  snprintf(content,
           sizeof(content),
           "%s %s (%s) %s\n",
           KERNEL_NAME,
           KERNEL_VERSION,
           KERNEL_GIT_HASH,
           KERNEL_TARGET);

  proc_copy_out(buf, size, offset, content, strlen(content));

  return 0;
}

int proc_version_init()
{
  proc_fs_register_readonly_file("version", &proc_read_version);

  return 0;
}

initcall_fs_register(proc_version_init);
