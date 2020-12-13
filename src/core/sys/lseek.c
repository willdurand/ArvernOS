#include "logging.h"
#include "syscall.h"
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/types.h>

void syscall_lseek(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;
  off_t offset = (off_t)registers->rcx;
  int whence = (int)registers->rsi;

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

  switch (whence) {
    case SEEK_SET:
      if (offset > stat.size) {
        registers->rdx = -1;
        errno = EINVAL;
        return;
      }

      desc->offset = offset;
      break;
    case SEEK_CUR:
      if (desc->offset + offset > stat.size) {
        registers->rdx = -1;
        errno = EINVAL;
        return;
      }

      desc->offset += offset;
      break;
    case SEEK_END:
      // TODO: implement me
    default:
      registers->rdx = -1;
      errno = EINVAL;
      return;
  }

  registers->rdx = desc->offset;
}
