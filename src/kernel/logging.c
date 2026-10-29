#include <logging.h>

#include <arch/io.h>

// Internal structure for the logging (ring) buffer.
typedef struct logging_buffer
{
  char buffer[LOGGING_BUFSIZE];
  size_t head;
  size_t tail;
} logging_buffer_t;

static void logging_buffer_putchar(char c);

static logging_buffer_t kmsg = { 0 };

bool kernel_boot_complete = false;
static log_level_t minimum_log_level = LEVEL_WARN;

void logging_stream_output(char c, void* arg)
{
  log_context_t* ctx = (log_context_t*)arg;

  logging_buffer_putchar(c);

  if (!kernel_boot_complete) {
    arch_serial_putchar(c);
    arch_screen_putchar(c);
    return;
  }

  if (ctx == NULL || ctx->level >= minimum_log_level) {
    arch_serial_putchar(c);
  }
}

static void logging_buffer_putchar(char c)
{
  if ((kmsg.head - kmsg.tail) == LOGGING_BUFSIZE) {
    kmsg.tail++;
  }

  kmsg.buffer[kmsg.head & (LOGGING_BUFSIZE - 1)] = c;
  kmsg.head++;
}

void logging_buffer_read(char* buf, size_t size, uint64_t offset)
{
  if (buf == NULL || size == 0) {
    return;
  }

  if (size > LOGGING_BUFSIZE) {
    size = LOGGING_BUFSIZE;
  }

  size_t head = kmsg.head;
  size_t tail = kmsg.tail;

  // Available bytes in the ring buffer.
  size_t available = (head >= tail) ? (head - tail) : 0;

  if (offset >= available) {
    buf[0] = '\0';
    return;
  }

  size_t start = tail + (size_t)offset;
  size_t read = 0;

  // Non-destructive read: do not advance kmsg.tail.
  while (read + 1 < size && (start + read) != head) {
    buf[read] = kmsg.buffer[(start + read) & (LOGGING_BUFSIZE - 1)];
    read++;
  }

  buf[read] = '\0';
}
