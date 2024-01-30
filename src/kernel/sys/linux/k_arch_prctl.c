#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <errno.h>
#include <sys/logging.h>

#define SET_FSBASE 0x1002

int k_arch_prctl(int code, uintptr_t addr)
{
  SYS_DEBUG("code=%d addr=%p", code, addr);

#ifdef __x86_64__
  if (code != SET_FSBASE) {
    return -EINVAL;
  }

  __asm__("wrfsbase %0" ::"r"(addr));

  return 0;
#else
  UNUSED(code);
  UNUSED(addr);

  return -EINVAL;
#endif
}
