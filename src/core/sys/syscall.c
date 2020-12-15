#include "syscall.h"
#include "logging.h"
#include <kernel/panic.h>
#include <sys/syscall.h>

// `+ 1` because it is a 1-based index.
static syscall_handler_t syscall_handlers[NB_SYSCALLS + 1] = { 0 };

void syscall_register_handler(uint8_t id, syscall_handler_t handler);
void syscall_open(registers_t* registers);
void syscall_close(registers_t* registers);
void syscall_read(registers_t* registers);
void syscall_lseek(registers_t* registers);
void syscall_fstat(registers_t* registers);
void syscall_gettimeofday(registers_t* registers);

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

void syscall_open(registers_t* registers)
{
  const char* pathname = (const char*)registers->rbx;
  uint32_t flags = registers->rcx;

  registers->rdx = k_open(pathname, flags);
}

void syscall_close(registers_t* registers)
{
  int fd = (int)registers->rbx;

  registers->rdx = k_close(fd);
}

void syscall_read(registers_t* registers)
{
  int fd = (int)registers->rbx;
  void* buf = (char*)registers->rcx;
  size_t count = (size_t)registers->rsi;

  registers->rdx = k_read(fd, buf, count);
}

void syscall_lseek(registers_t* registers)
{
  int fd = (int)registers->rbx;
  off_t offset = (off_t)registers->rcx;
  int whence = (int)registers->rsi;

  registers->rdx = k_lseek(fd, offset, whence);
}

void syscall_fstat(registers_t* registers)
{
  int fd = (int)registers->rbx;
  struct stat* statbuf = (struct stat*)registers->rcx;

  registers->rdx = k_fstat(fd, statbuf);
}

void syscall_gettimeofday(registers_t* registers)
{
  struct timeval* t = (struct timeval*)registers->rbx;
  void* z = (void*)registers->rcx;

  registers->rdx = k_gettimeofday(t, z);
}
