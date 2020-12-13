#include "syscall.h"
#include "logging.h"
#include <drivers/keyboard.h>
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <kernel/console.h>
#include <kernel/panic.h>
#include <net/socket.h>
#include <proc/descriptor.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>

// `+ 1` because it is a 1-based index.
static syscall_handler_t syscall_handlers[NB_SYSCALLS + 1] = { 0 };

void syscall_register_handler(uint8_t id, syscall_handler_t handler);

void syscall_init()
{
  syscall_register_handler(SYSCALL_TEST, syscall_test);
  syscall_register_handler(SYSCALL_WRITE, syscall_write);
  syscall_register_handler(SYSCALL_READ, syscall_read);
  syscall_register_handler(SYSCALL_GETTIMEOFDAY, syscall_gettimeofday);
  syscall_register_handler(SYSCALL_OPEN, syscall_open);
  syscall_register_handler(SYSCALL_CLOSE, syscall_close);
  syscall_register_handler(SYSCALL_REBOOT, syscall_reboot);
  syscall_register_handler(SYSCALL_FSTAT, syscall_fstat);
  syscall_register_handler(SYSCALL_LSEEK, syscall_lseek);
  syscall_register_handler(SYSCALL_SOCKET, syscall_socket);
  syscall_register_handler(SYSCALL_SENDTO, syscall_sendto);
  syscall_register_handler(SYSCALL_RECVFROM, syscall_recvfrom);
}

void syscall_register_handler(uint8_t id, syscall_handler_t handler)
{
  syscall_handlers[id] = handler;
}

void syscall_handler(registers_t* registers)
{
  syscall_handler_t handler = syscall_handlers[registers->rax];

  if (handler != 0) {
    handler(registers);
    return;
  }

  PANIC("Received unimplemented syscall: %d\n", registers->rax);
}

void syscall_write(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;
  char* buf = (char*)registers->rcx;
  size_t count = (size_t)registers->rsi;

  if (fd == STDOUT || fd == STDERR) {
    registers->rdx = console_write(buf, count);
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

  if ((desc->flags != O_WRONLY && desc->flags != O_RDWR) ||
      desc->flags == O_RDONLY) {
    CORE_SYS_DEBUG("invalid flags for file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  registers->rdx = vfs_write(desc->inode, buf, count, desc->offset);
}

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

void syscall_open(registers_t* registers)
{
  errno = 0;

  const char* pathname = (const char*)registers->rbx;
  uint32_t flags = registers->rcx;

  inode_t inode = vfs_namei(pathname);

  if (inode == 0) {
    registers->rdx = -1;
    errno = ENOENT;
    return;
  }

  int fd = create_file_descriptor(inode, flags);

  if (fd == -1) {
    CORE_SYS_DEBUG("%s", "too many files open");
    registers->rdx = -1;
    errno = ENFILE;
    return;
  }

  registers->rdx = fd;

  CORE_SYS_DEBUG("open fd=%d inode=%p flags=%d", registers->rdx, inode, flags);
}

void syscall_close(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;

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

  if (desc->inode != NULL) {
    registers->rdx = vfs_close(desc->inode);
  }

  if (desc->port != 0) {
    registers->rdx = socket_delete_buffer(fd);
  }

  delete_descriptor(fd);

  CORE_SYS_DEBUG("close fd=%d", fd);
}
