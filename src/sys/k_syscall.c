#include "k_syscall.h"
#include "logging.h"
#include <kernel/panic.h>
#include <sys/syscall.h>

// `+ 1` because it is a 1-based index.
static syscall_handler_t syscall_handlers[NB_SYSCALLS + 1] = { 0 };

void syscall_register_handler(uint8_t id, syscall_handler_t handler);
void syscall_open(isr_stack_t* stack);
void syscall_close(isr_stack_t* stack);
void syscall_read(isr_stack_t* stack);
void syscall_lseek(isr_stack_t* stack);
void syscall_fstat(isr_stack_t* stack);
void syscall_gettimeofday(isr_stack_t* stack);
void syscall_reboot(isr_stack_t* stack);
void syscall_sendto(isr_stack_t* stack);
void syscall_socket(isr_stack_t* stack);
void syscall_write(isr_stack_t* stack);
void syscall_test(isr_stack_t* stack);
void syscall_recvfrom(isr_stack_t* stack);
void syscall_gethostbyname2(isr_stack_t* stack);

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
  syscall_register_handler(SYSCALL_GETHOSTBYNAME2, syscall_gethostbyname2);
}

void syscall_register_handler(uint8_t id, syscall_handler_t handler)
{
  syscall_handlers[id] = handler;
}

void syscall_handler(isr_stack_t* stack)
{
  syscall_handler_t handler = syscall_handlers[stack->rax];

  if (handler != 0) {
    handler(stack);
    return;
  }

  PANIC("Received unimplemented syscall: %d\n", stack->rax);
}

void syscall_open(isr_stack_t* stack)
{
  const char* pathname = (const char*)stack->rbx;
  uint32_t flags = stack->rcx;

  stack->rdx = k_open(pathname, flags);
}

void syscall_close(isr_stack_t* stack)
{
  int fd = (int)stack->rbx;

  stack->rdx = k_close(fd);
}

void syscall_read(isr_stack_t* stack)
{
  int fd = (int)stack->rbx;
  void* buf = (char*)stack->rcx;
  size_t count = (size_t)stack->rsi;

  stack->rdx = k_read(fd, buf, count);
}

void syscall_lseek(isr_stack_t* stack)
{
  int fd = (int)stack->rbx;
  off_t offset = (off_t)stack->rcx;
  int whence = (int)stack->rsi;

  stack->rdx = k_lseek(fd, offset, whence);
}

void syscall_fstat(isr_stack_t* stack)
{
  int fd = (int)stack->rbx;
  struct stat* statbuf = (struct stat*)stack->rcx;

  stack->rdx = k_fstat(fd, statbuf);
}

void syscall_gettimeofday(isr_stack_t* stack)
{
  struct timeval* t = (struct timeval*)stack->rbx;
  void* z = (void*)stack->rcx;

  stack->rdx = k_gettimeofday(t, z);
}

void syscall_reboot(isr_stack_t* stack)
{
  int command = (int)stack->rbx;

  stack->rdx = k_reboot(command);
}

void syscall_sendto(isr_stack_t* stack)
{
  int sockfd = (int)stack->rsi;
  const void* buf = (const void*)stack->rdx;
  size_t len = (size_t)stack->rcx;
  int flags = (int)stack->r10;
  const struct sockaddr* dst_addr = (const struct sockaddr*)stack->r8;
  socklen_t addrlen = (socklen_t)stack->r9;

  stack->rdx = k_sendto(sockfd, buf, len, flags, dst_addr, addrlen);
}

void syscall_socket(isr_stack_t* stack)
{
  int domain = (int)stack->rbx;
  int type = (int)stack->rcx;
  int protocol = (int)stack->rsi;

  stack->rdx = k_socket(domain, type, protocol);
}

void syscall_write(isr_stack_t* stack)
{
  int fd = (int)stack->rbx;
  const char* buf = (char*)stack->rcx;
  size_t count = (size_t)stack->rsi;

  stack->rdx = k_write(fd, buf, count);
}

void syscall_test(isr_stack_t* stack)
{
  const char* s = (const char*)stack->rbx;

  k_test(s);
}

void syscall_recvfrom(isr_stack_t* stack)
{
  int sockfd = (int)stack->rsi;
  void* buf = (void*)stack->rdx;
  size_t len = (size_t)stack->rcx;
  int flags = (int)stack->r10;
  struct sockaddr* src_addr = (struct sockaddr*)stack->r8;
  socklen_t* addrlen = (socklen_t*)stack->r9;

  stack->rdx = k_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

void syscall_gethostbyname2(isr_stack_t* stack)
{
  const char* name = (const char*)stack->rsi;
  struct in_addr* in = (struct in_addr*)stack->rdx;

  stack->rdx = k_gethostbyname2(name, in);
}
