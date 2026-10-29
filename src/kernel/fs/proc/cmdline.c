#include <fs/proc.h>

#include <initcall.h>
#include <string.h>

// This variable is defined in `src/kernel/kmain.c`.
extern char* saved_cmdline;

int proc_read_cmdline(char* buf, size_t size, uint64_t offset)
{
  if (size == 0) {
    return 0;
  }

  size_t cmd_len = (saved_cmdline != NULL) ? strlen(saved_cmdline) : 0;
  // The content ends with a newline.
  size_t len = cmd_len + 1;

  if (offset >= len) {
    buf[0] = '\0';
    return 0;
  }

  size_t count = len - (size_t)offset;
  if (count >= size) {
    count = size - 1;
  }

  // Copy the command line bytes in the requested window. A buffer of
  // `count + 1` makes `proc_copy_out()` copy at most `count` bytes.
  size_t copied = proc_copy_out(buf, count + 1, offset, saved_cmdline, cmd_len);

  // Append the trailing newline if the window reaches the end of the command
  // line.
  if (copied < count) {
    buf[copied] = '\n';
    buf[copied + 1] = '\0';
  }

  return 0;
}

int proc_cmdline_init()
{
  proc_fs_register_readonly_file("cmdline", &proc_read_cmdline);

  return 0;
}

initcall_fs_register(proc_cmdline_init);
