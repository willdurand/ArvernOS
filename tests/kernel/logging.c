#include <logging.h>

#include <string.h>
#include <test.h>

void arch_screen_putchar(char c)
{
  // noop implementation
}

void arch_serial_putchar(char c)
{
  // noop implementation
}

static void write_log(const char* s)
{
  for (size_t i = 0; s[i] != '\0'; i++) {
    logging_stream_output(s[i], NULL);
  }
}

static bool is_filled_with(const char* s, size_t len, char c)
{
  for (size_t i = 0; i < len; i++) {
    if (s[i] != c) {
      return false;
    }
  }

  return true;
}

int main()
{
  describe("logging_buffer_read()");
  {
    char buf[32];

    logging_buffer_read(buf, sizeof(buf), 0);
    assert(strcmp(buf, "") == 0,
           "returns an empty string when the buffer is empty");

    write_log("hello");

    logging_buffer_read(buf, sizeof(buf), 0);
    assert(strcmp(buf, "hello") == 0, "reads the log buffer");

    logging_buffer_read(buf, sizeof(buf), 1);
    assert(strcmp(buf, "ello") == 0, "reads the log buffer at an offset");

    logging_buffer_read(buf, sizeof(buf), 5);
    assert(strcmp(buf, "") == 0,
           "returns an empty string at the end of the log buffer");

    logging_buffer_read(buf, sizeof(buf), 6);
    assert(strcmp(buf, "") == 0,
           "returns an empty string past the end of the log buffer");

    logging_buffer_read(buf, 3, 0);
    assert(strcmp(buf, "he") == 0, "reads at most size - 1 bytes");

    logging_buffer_read(buf, 1, 0);
    assert(strcmp(buf, "") == 0, "handles a one-byte buffer");
  }
  end_describe();

  describe("logging_buffer_read() with a full ring buffer");
  {
    // Fill the whole ring buffer and then some, so that the oldest bytes are
    // dropped.
    for (size_t i = 0; i < LOGGING_BUFSIZE + 1000; i++) {
      logging_stream_output('a', NULL);
    }

    char content[LOGGING_BUFSIZE];

    logging_buffer_read(content, sizeof(content), 0);
    assert(strlen(content) == LOGGING_BUFSIZE - 1,
           "reads at most LOGGING_BUFSIZE - 1 bytes");
    assert(is_filled_with(content, LOGGING_BUFSIZE - 1, 'a'),
           "reads the log data");

    logging_buffer_read(content, sizeof(content), LOGGING_BUFSIZE - 1);
    assert(strlen(content) == 1, "reads the last byte of a full ring buffer");
    assert(content[0] == 'a', "reads the last byte of the log data");

    logging_buffer_read(content, sizeof(content), LOGGING_BUFSIZE);
    assert(strcmp(content, "") == 0,
           "returns an empty string past the ring buffer");
  }
  end_describe();

  return test_summary();
}
