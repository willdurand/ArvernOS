#include "logging.h"
#include "syscall.h"
#include <errno.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

void syscall_fstat(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;
  struct stat* statbuf = (struct stat*)registers->rcx;

  if (fd < 3) {
    CORE_SYS_DEBUG("invalid file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EPERM;
    return;
  }

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("file descriptor fd=%d not found", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  stat_t stat;
  vfs_stat(desc->inode, &stat);
  statbuf->st_size = stat.size;

  registers->rdx = 0;
}
