#include <arch/logging.h>

#include <arvern/utils.h>
#include <drivers/miniuart.h>

void arch_logging_stream_output(char c, void* arg)
{
  UNUSED(arg);

  miniuart_putc(c);
}
