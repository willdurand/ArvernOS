#include <errno.h>

#include <arvern/utils.h>

int errno = 0;

char* strerror(int errnum)
{
  UNUSED(errnum);

  return "(unimplemented)";
}
