#include "syscall.h"
#include <core/cmos.h>
#include <core/reboot.h>
#include <core/timer.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <errno.h>
#include <fs/vfs.h>
#include <kernel/panic.h>
#include <proc/fd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

syscall_handler_t syscall_handlers[NB_SYSCALLS];

void syscall_register_handler(uint8_t id, syscall_handler_t handler);
void syscall_print_registers(registers_t* registers);

void syscall_test(registers_t* registers);
void syscall_write(registers_t* registers);
void syscall_read(registers_t* registers);
void syscall_gettimeofday(registers_t* registers);
void syscall_open(registers_t* registers);
void syscall_close(registers_t* registers);
void syscall_reboot(registers_t* registers);

void syscall_init()
{
  syscall_register_handler(SYSCALL_TEST, syscall_test);
  syscall_register_handler(SYSCALL_WRITE, syscall_write);
  syscall_register_handler(SYSCALL_READ, syscall_read);
  syscall_register_handler(SYSCALL_GETTIMEOFDAY, syscall_gettimeofday);
  syscall_register_handler(SYSCALL_OPEN, syscall_open);
  syscall_register_handler(SYSCALL_CLOSE, syscall_close);
  syscall_register_handler(SYSCALL_REBOOT, syscall_reboot);
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

void syscall_test(registers_t* registers)
{
  printf("    (syscall_test) hello, %s!\n", registers->rbx);
}

void syscall_write(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;
  char* buf = (char*)registers->rcx;
  size_t count = (size_t)registers->rsi;

  if (fd == FD_STDOUT || fd == FD_STDERR) {
    registers->rdx = screen_print(buf, count);
    return;
  }

  if (fd < 3) {
    DEBUG("invalid file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EPERM;
    return;
  }

  DEBUG("fd=%d buf=%p count=%d", fd, buf, count);

  file_descriptor_t* desc = get_file_descriptor(fd);

  if (desc == 0) {
    DEBUG("file descriptor fd=%d not found", fd);
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

  if (fd == FD_STDIN) {
    uint8_t scancode = keyboard_get_scancode();

    if (scancode) {
      buf[0] = scancode;
      registers->rdx = 1;
    }

    return;
  }

  if (fd < 3) {
    DEBUG("invalid file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EPERM;
    return;
  }

  DEBUG("fd=%d buf=%p count=%d", fd, buf, count);

  file_descriptor_t* desc = get_file_descriptor(fd);

  if (desc == 0) {
    DEBUG("file descriptor fd=%d not found", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  registers->rdx = vfs_read(desc->inode, buf, count, desc->offset);
}

void syscall_gettimeofday(registers_t* registers)
{
  struct timeval* t = registers->rbx;

  t->tv_sec = cmos_boot_time() + timer_uptime();
  // TODO(william): set a correct value, see:
  // https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
  t->tv_usec = 0;

  DEBUG("gettimeofday=%u", t->tv_sec);
}

void syscall_open(registers_t* registers)
{
  errno = 0;

  const char* pathname = registers->rbx;
  uint32_t flags = registers->rcx;

  inode_t inode = vfs_namei(pathname);

  if (inode == 0) {
    registers->rdx = -2;
    errno = ENOENT;
    return;
  }

  registers->rdx = create_file_descriptor(inode, flags);

  DEBUG("open fd=%d inode=%p flags=%d", registers->rdx, inode, flags);
}

void syscall_close(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;

  if (fd < 3) {
    DEBUG("invalid file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EPERM;
    return;
  }

  file_descriptor_t* desc = get_file_descriptor(fd);

  if (desc == 0) {
    DEBUG("file descriptor fd=%d not found", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  registers->rdx = vfs_close(desc->inode);
  delete_file_descriptor(fd);

  DEBUG("close fd=%d", fd);
}

void syscall_reboot(registers_t* registers)
{
  int command = (int)registers->rbx;

  DEBUG("reboot command=%d", command);

  registers->rdx = kreboot(command);
}

void syscall_print_registers(registers_t* registers)
{
  printf("registers:\n");
  printf("  rax=%#x\n", registers->rax);
  printf("  rbx=%#x\n", registers->rbx);
  printf("  rcx=%#x\n", registers->rcx);
  printf("  rdx=%#x\n", registers->rdx);
  printf("  rsi=%#x\n", registers->rsi);
  printf("  rdi=%#x\n", registers->rdi);
  printf("  rbp=%#x\n", registers->rbp);
  printf("   r8=%#x\n", registers->r8);
  printf("   r9=%#x\n", registers->r9);
  printf("  r10=%#x\n", registers->r10);
  printf("  r11=%#x\n", registers->r11);
  printf("  r12=%#x\n", registers->r12);
  printf("  r13=%#x\n", registers->r13);
  printf("  r14=%#x\n", registers->r14);
  printf("  r15=%#x\n", registers->r15);
}
