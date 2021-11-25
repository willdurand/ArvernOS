#include <logging.h>

#include <arch/logging.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

void logging_impl(uint8_t level,
                  uint8_t minimum_level,
                  source_location_t loc,
                  const char* format,
                  ...)
{
  if (level < minimum_level) {
    return;
  }

  va_list args;
  va_start(args, format);

  // We output ndjson structured logs.
  fctprintf(&arch_logging_stream_output,
            NULL,
            "{\"level\":%d,\"caller\":\"%s:%" PRIu64 ":%s()\",\"msg\":\"",
            level,
            loc.file,
            loc.line,
            loc.func);
  fctvprintf(&arch_logging_stream_output, NULL, format, args);
  fctprintf(&arch_logging_stream_output, NULL, "\"}\n");

  va_end(args);
}
