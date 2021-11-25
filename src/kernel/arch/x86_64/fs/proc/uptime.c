#include <fs/proc.h>

#include <drivers/timer.h>
#include <init.h>
#include <inttypes.h>
#include <stdio.h>

int proc_read_uptime(char* buf, size_t size)
{
  // Linux returns two numbers according to the man page (quoted below):
  // http://man7.org/linux/man-pages/man5/proc.5.html.
  //
  // [/proc/uptime] contains two numbers (values in seconds): the uptime of
  // the system (including time spent in suspend) and the amount of time
  // spent in the idle process.
  //
  // We do not have the notion of process yet so there is only one value.
  snprintf(buf, size, "%" PRIu64 "\n", timer_uptime());

  return 0;
}

int proc_uptime_init()
{
  proc_fs_register_readonly_file("uptime", &proc_read_uptime);

  return 0;
}

init_register(proc_uptime_init);
