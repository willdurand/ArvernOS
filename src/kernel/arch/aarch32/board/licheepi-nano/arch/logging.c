#include <arch/logging.h>

#include <arvern/utils.h>
#include <drivers/uart0.h>

void arch_logging_stream_output(char c, void* arg)
{
  UNUSED(*arg);

  uart0_putc(c);
}
