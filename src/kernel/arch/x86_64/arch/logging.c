#include <arch/logging.h>

#include <drivers/serial.h>
#include <willos/utils.h>

void arch_logging_stream_output(char c, void* arg)
{
  UNUSED(*arg);

  serial_write(SERIAL_COM1, c);
}
