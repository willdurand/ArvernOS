#include "k_syscall.h"
#include "logging.h"
#include <core/gdt.h>
#include <core/register.h>
#include <sys/syscall.h>

int k_not_implemented();

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
  [SYSCALL_YIELD] = (syscall_ptr_t)k_yield,
};

void syscall_init()
{
  // Enable IA32_EFER.SCE.
  write_msr(IA32_EFER, read_msr(IA32_EFER) | 1);
  // Set up the segments for syscall/sysret.
  uint64_t star = read_msr(IA32_STAR);
  star |= (uint64_t)KERNEL_BASE_SELECTOR << 32;
  star |= (uint64_t)USER_BASE_SELECTOR << 48;
  write_msr(IA32_STAR, star);
  write_msr(IA32_LSTAR, (uint64_t)&syscall_handler);
  write_msr(IA32_SFMASK, 0);

  SYS_DEBUG("IA32_EFER=0x%016x IA32_STAR=0x%016x IA32_LSTAR=0x%016x",
            read_msr(IA32_EFER),
            read_msr(IA32_STAR),
            read_msr(IA32_LSTAR));
}

int k_not_implemented()
{
  return -ENOSYS;
}
