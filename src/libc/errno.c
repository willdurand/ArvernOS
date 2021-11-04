#include <errno.h>

int errno = 0;

char* strerror(int errnum)
{
  (void)(errnum);

  return "(unimplemented)";
}
