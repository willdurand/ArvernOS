#include "k_syscall.h"
#include "logging.h"
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <kernel/console.h>
#include <proc/descriptor.h>
#include <stddef.h>

ssize_t k_write(int fd, const void* buf, size_t count)
{
  if (fd == STDOUT || fd == STDERR) {
    return console_write(buf, count);
  }

  if (fd < 3) {
    CORE_SYS_DEBUG("invalid file descriptor fd=%d", fd);
    return -EPERM;
  }

  CORE_SYS_DEBUG("fd=%d buf=%p count=%d", fd, buf, count);

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("file descriptor fd=%d not found", fd);
    return -EBADF;
  }

  if ((desc->flags != O_WRONLY && desc->flags != O_RDWR) ||
      desc->flags == O_RDONLY) {
    CORE_SYS_DEBUG("invalid flags for file descriptor fd=%d", fd);
    return -EBADF;
  }

  return vfs_write(desc->inode, buf, count, desc->offset);
}
