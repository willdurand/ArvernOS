#include <arch/sys.h>

#include <core/gdt.h>
#include <core/register.h>
#include <sys/logging.h>

// This is defined in `src/kernel/arch/x86_64/asm/k_syscall.asm`.
extern void syscall_handler();

void arch_syscall_init()
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
