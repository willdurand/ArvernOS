#include <arch/io.h>
#include <arch/logging.h>

#include <arvern/utils.h>

void arch_logging_stream_output(char c, void* arg)
{
  UNUSED(arg);

  // The log stream shares the console output so that debugging stuff are
  // visible on the screen and captured on the serial port without being
  // printed twice.
  arch_putchar(c);
}
