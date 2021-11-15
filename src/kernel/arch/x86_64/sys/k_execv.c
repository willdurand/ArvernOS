#include "k_syscall.h"
#include <core/elf.h>
#include <core/isr.h>
#include <fcntl.h>
#include <proc/process.h>
#include <proc/usermode.h>
#include <stdlib.h>
#include <string.h>
#include <sys/logging.h>
#include <sys/stat.h>

int k_execv(const char* path, char* const argv[])
{
  int fd = k_open(path, O_RDONLY);
  if (fd < 3) {
    SYS_DEBUG("%s", "error k_open");
    return fd;
  }

  struct stat stat = { 0 };

  int retval = k_fstat(fd, &stat);
  if (retval != 0) {
    SYS_DEBUG("%s", "error k_fstat");
    return retval;
  }

  uint8_t* buf = (uint8_t*)malloc(stat.st_size * sizeof(uint8_t));

  retval = k_read(fd, buf, stat.st_size);
  if (retval != stat.st_size) {
    SYS_DEBUG("%s", "error k_read");
    return retval;
  }

  retval = k_close(fd);
  if (retval != 0) {
    SYS_DEBUG("%s", "error k_close");
    return retval;
  }

  process_t* current_process = process_exec(buf, path, argv);

  SYS_DEBUG("switching to process '%s': entrypoint=%p user_rsp=%p",
            current_process->name,
            current_process->elf->entry,
            current_process->user_rsp);

  switch_to_usermode((void*)current_process->elf->entry,
                     (void*)current_process->user_rsp);

  // Should not be possible.
  return 0;
}
