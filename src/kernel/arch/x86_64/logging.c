#include <logging.h>

#include <drivers/serial.h>
#include <willos/utils.h>

void logging_stream_output(char c, void* arg)
{
  UNUSED(*arg);

  serial_write(SERIAL_COM1, c);
}
