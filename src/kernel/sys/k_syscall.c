#include <sys/k_syscall.h>

#include <arch/sys.h>
#include <sys/syscall.h>

typedef void (*syscall_ptr_t)(void);

syscall_ptr_t syscall_handlers[] = {
  (syscall_ptr_t)k_not_implemented,
  [SYSCALL_TEST] = (syscall_ptr_t)k_test,
  [SYSCALL_WRITE] = (syscall_ptr_t)k_write,
  [SYSCALL_READ] = (syscall_ptr_t)k_read,
  [SYSCALL_GETTIMEOFDAY] = (syscall_ptr_t)k_gettimeofday,
  [SYSCALL_OPEN] = (syscall_ptr_t)k_open,
  [SYSCALL_CLOSE] = (syscall_ptr_t)k_close,
  [SYSCALL_REBOOT] = (syscall_ptr_t)k_reboot,
  [SYSCALL_FSTAT] = (syscall_ptr_t)k_fstat,
  [SYSCALL_LSEEK] = (syscall_ptr_t)k_lseek,
  [SYSCALL_SOCKET] = (syscall_ptr_t)k_socket,
  [SYSCALL_SENDTO] = (syscall_ptr_t)k_sendto,
  [SYSCALL_RECVFROM] = (syscall_ptr_t)k_recvfrom,
  [SYSCALL_GETHOSTBYNAME2] = (syscall_ptr_t)k_gethostbyname2,
  [SYSCALL_EXECV] = (syscall_ptr_t)k_execv,
  [SYSCALL_GETPID] = (syscall_ptr_t)k_getpid,
};

void syscall_init()
{
  arch_syscall_init();
}

int k_not_implemented()
{
  return -ENOSYS;
}
