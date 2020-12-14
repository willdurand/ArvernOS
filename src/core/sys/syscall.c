#include "syscall.h"
#include "logging.h"
#include <kernel/panic.h>
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
