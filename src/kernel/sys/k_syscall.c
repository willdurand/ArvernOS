#include <sys/k_syscall.h>

#include <arch/sys.h>
#include <sys/logging.h>
#include <sys/syscall.h>

typedef void (*syscall_ptr_t)(void);

syscall_ptr_t syscall_handlers[300] = {
  [0 ... 299] = (syscall_ptr_t)k_not_implemented,
};

void syscall_init()
{
  INFO("%s", "sys: initialize syscalls");

  syscall_handlers[SYSCALL_TEST] = (syscall_ptr_t)k_test;
  syscall_handlers[SYSCALL_WRITE] = (syscall_ptr_t)k_write;
  syscall_handlers[SYSCALL_READ] = (syscall_ptr_t)k_read;
  syscall_handlers[SYSCALL_GETTIMEOFDAY] = (syscall_ptr_t)k_gettimeofday;
  syscall_handlers[SYSCALL_OPEN] = (syscall_ptr_t)k_open;
  syscall_handlers[SYSCALL_CLOSE] = (syscall_ptr_t)k_close;
  syscall_handlers[SYSCALL_REBOOT] = (syscall_ptr_t)k_reboot;
  syscall_handlers[SYSCALL_FSTAT] = (syscall_ptr_t)k_fstat;
  syscall_handlers[SYSCALL_LSEEK] = (syscall_ptr_t)k_lseek;
  syscall_handlers[SYSCALL_SOCKET] = (syscall_ptr_t)k_socket;
  syscall_handlers[SYSCALL_SENDTO] = (syscall_ptr_t)k_sendto;
  syscall_handlers[SYSCALL_RECVFROM] = (syscall_ptr_t)k_recvfrom;
  syscall_handlers[SYSCALL_GETHOSTBYNAME2] = (syscall_ptr_t)k_gethostbyname2;
  syscall_handlers[SYSCALL_EXECV] = (syscall_ptr_t)k_execv;
  syscall_handlers[SYSCALL_GETPID] = (syscall_ptr_t)k_getpid;
  syscall_handlers[SYSCALL_EXIT] = (syscall_ptr_t)k_exit;
  syscall_handlers[SYSCALL_OPENAT] = (syscall_ptr_t)k_openat;

  arch_syscall_init();
}

int k_not_implemented()
{
  return -ENOSYS;
}
