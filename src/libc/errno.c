#include <errno.h>
#include <willos/utils.h>

int errno = 0;

char* strerror(int errnum)
{
  UNUSED(errnum);

  return "(unimplemented)";
}
