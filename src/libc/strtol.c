#include <stdlib.h>

// TODO: This function is supposed to implement `strtol()` but it has been
// written for the kernel stacktrace and does not behave like it should (e.g.,
// it does not skip spaces and does not support a `base` different than `16`).
long int strtol_wip(const char* str, char** end, int base)
{
  char* ptr = (char*)str;
  long int value = 0;

  if (base != 16) {
    return value;
  }

  while (*ptr != '\0') {
    uint8_t byte = *ptr;

    if (byte >= '0' && byte <= '9') {
      byte = byte - '0';
    } else if (byte >= 'a' && byte <= 'f') {
      byte = byte - 'a' + 10;
    } else if (byte >= 'A' && byte <= 'F') {
      byte = byte - 'A' + 10;
    } else {
      if (end != NULL) {
        *end = ptr;
      }

      return value;
    }

    value = (value << 4) | (byte & 0xF);
    ptr++;
  }

  return value;
}
