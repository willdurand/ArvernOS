#include "k_syscall.h"
#include "logging.h"
#include <drivers/keyboard.h>
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>

ssize_t k_read(int fd, void* buf, size_t count)
{
  if (fd == STDIN) {
    uint8_t scancode = keyboard_get_scancode();

    if (scancode) {
      ((char*)buf)[0] = scancode;
      return 1;
    }

    return 0;
  }

  if (fd < 3) {
    SYS_DEBUG("invalid file descriptor fd=%d", fd);
    return -EPERM;
  }

  SYS_DEBUG("fd=%d buf=%p count=%d", fd, buf, count);

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    SYS_DEBUG("file descriptor fd=%d not found", fd);
    return -EBADF;
  }

  if ((desc->flags != O_RDONLY && desc->flags != O_RDWR) ||
      desc->flags == O_WRONLY) {
    SYS_DEBUG("invalid flags for file descriptor fd=%d", fd);
    return -EBADF;
  }

  ssize_t bytes_read = vfs_read(desc->inode, buf, count, desc->offset);

  desc->offset += bytes_read;

  return bytes_read;
}
