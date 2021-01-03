#include <willos/debug.h>

#ifndef __is_libk

#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>

void willos_debug_putchar(char c, void* arg)
{
  int fd = *(int*)arg;
  write(fd, &c, 1);
}

void willos_debug(const char* fmt, ...)
{
  int fd = open("/dev/com1", O_WRONLY);

  if (fd < 3) {
    return;
  }

  va_list arg;
  va_start(arg, fmt);
  fctprintf(&willos_debug_putchar, (void*)&fd, fmt, arg);
  va_end(arg);

  close(fd);
}

#endif
