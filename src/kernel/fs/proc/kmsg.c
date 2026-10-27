#include <fs/proc.h>

#include <initcall.h>
#include <logging.h>

int proc_read_kmsg(char* buf, size_t size, uint64_t offset)
{
  logging_buffer_read(buf, size, offset);

  return 0;
}

int proc_kmsg_init()
{
  proc_fs_register_readonly_file("kmsg", &proc_read_kmsg);

  return 0;
}

initcall_fs_register(proc_kmsg_init);
