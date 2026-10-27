#include <fs/proc.h>

#include <initcall.h>
#include <stdio.h>
#include <string.h>

// This variable is defined in `src/kernel/kmain.c`.
extern char* saved_cmdline;

int proc_read_cmdline(char* buf, size_t size, uint64_t offset)
{
  snprintf(buf, size, "%s\n", saved_cmdline);

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

int proc_cmdline_init()
{
  proc_fs_register_readonly_file("cmdline", &proc_read_cmdline);

  return 0;
}

initcall_fs_register(proc_cmdline_init);
