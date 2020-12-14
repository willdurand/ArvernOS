#include "logging.h"
#include "syscall.h"
#include <drivers/keyboard.h>
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/types.h>

void syscall_read(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;
  char* buf = (char*)registers->rcx;
  size_t count = (size_t)registers->rsi;

  if (fd == STDIN) {
    uint8_t scancode = keyboard_get_scancode();

    if (scancode) {
      buf[0] = scancode;
      registers->rdx = 1;
    }

    return;
  }

  if (fd < 3) {
    CORE_SYS_DEBUG("invalid file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EPERM;
    return;
  }

  CORE_SYS_DEBUG("fd=%d buf=%p count=%d", fd, buf, count);

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("file descriptor fd=%d not found", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  if ((desc->flags != O_RDONLY && desc->flags != O_RDWR) ||
      desc->flags == O_WRONLY) {
    CORE_SYS_DEBUG("invalid flags for file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  ssize_t bytes_read = vfs_read(desc->inode, buf, count, desc->offset);
  desc->offset += bytes_read;
  registers->rdx = bytes_read;
}
