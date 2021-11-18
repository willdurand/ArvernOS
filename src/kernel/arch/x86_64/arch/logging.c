#include <arch/logging.h>

#include <arvern/utils.h>
#include <drivers/serial.h>

void arch_logging_stream_output(char c, void* arg)
{
  UNUSED(*arg);

  serial_write(SERIAL_COM1, c);
}
