#include <string.h>

int memcmp(const void* a_ptr, const void* b_ptr, size_t length)
{

  const unsigned char* a = a_ptr;
  const unsigned char* b = b_ptr;

  for (size_t i = 0; i < length; i++) {
    if (a[i] < b[i]) {
      return -1;
    } else if (a[i] > b[i]) {
      return 1;
    }
  }

  return 0;
}
