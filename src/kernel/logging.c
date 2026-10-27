#include <logging.h>

#include <arch/io.h>
#include <arvern/utils.h>

void logging_stream_output(char c, void* arg)
{
  UNUSED(arg);

  arch_putchar(c);
}
