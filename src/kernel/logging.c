#include <logging.h>

#include <arch/io.h>
#include <arvern/utils.h>

bool kernel_boot_complete = false;

void logging_stream_output(char c, void* arg)
{
  UNUSED(arg);

  if (!kernel_boot_complete) {
    arch_serial_putchar(c);
    arch_screen_putchar(c);
  } else {
    // TODO: introduce log levels and filter out non-critical messages.
    arch_serial_putchar(c);
  }
}
