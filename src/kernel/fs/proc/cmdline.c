#include <fs/proc.h>
#include <init.h>

#include <stdio.h>

// This variable is defined in `src/kernel/kmain.c`.
extern char* saved_cmdline;

int proc_read_cmdline(char* buf, size_t size)
{
  snprintf(buf, size, "%s\n", saved_cmdline);

  return 0;
}

int proc_cmdline_init()
{
  proc_fs_register_readonly_file("cmdline", &proc_read_cmdline);

  return 0;
}

init_register(proc_cmdline_init);
