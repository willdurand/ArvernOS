#include <logging.h>

#include <arch/io.h>

bool kernel_boot_complete = false;
log_level_t minimum_log_level = LEVEL_WARN;

void logging_stream_output(char c, void* arg)
{
  log_context_t* ctx = (log_context_t*)arg;

  if (!kernel_boot_complete) {
    arch_serial_putchar(c);
    arch_screen_putchar(c);
    return;
  }

  if (ctx == NULL || ctx->level >= minimum_log_level) {
    arch_serial_putchar(c);
  }
}
