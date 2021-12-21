#include <sys/k_syscall.h>

#include <arch/io.h>
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <string.h>
#include <sys/logging.h>

ssize_t k_write(int fd, const void* buf, size_t count)
{
  if (fd == STDOUT || fd == STDERR) {
    // TODO: Make the code in this block better.
    const char* s = (const char*)buf;
    if (count > strlen(s)) {
      count = strlen(s);
    }

    for (size_t i = 0; i < count; i++) {
      arch_putchar(s[i]);
    }

    return count;
  }

  if (fd < 3) {
    SYS_DEBUG("invalid file descriptor fd=%d", fd);
    return -EPERM;
  }

#ifndef ENABLE_USERLAND_DEBUG
  // This would conflict with the userland message being written.
  SYS_DEBUG("fd=%d buf=%p count=%d", fd, buf, count);
#endif

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    SYS_DEBUG("file descriptor fd=%d not found", fd);
    return -EBADF;
  }

  // Socket descriptors.
  if (desc->port != 0) {
    return vfs_write(desc->inode, (void*)buf, count, 0);
  }

  if ((desc->flags != O_WRONLY && desc->flags != O_RDWR) ||
      desc->flags == O_RDONLY) {
    SYS_DEBUG("invalid flags for file descriptor fd=%d", fd);
    return -EBADF;
  }

  return vfs_write(desc->inode, (void*)buf, count, desc->offset);
}
