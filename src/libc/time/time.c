#include <sys/syscall.h>
#include <time.h>

time_t time(time_t* second)
{
  struct timeval p;
  gettimeofday(&p, NULL);

  if (second) {
    *second = p.tv_sec;
  }

  return p.tv_sec;
}
