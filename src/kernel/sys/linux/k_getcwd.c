#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <string.h>
#include <sys/logging.h>

char* k_getcwd(char* buf, size_t size)
{
  UNUSED(size);

  if (buf != NULL) {
    strncpy(buf, "/", 2);
  }

  return "/";
}
