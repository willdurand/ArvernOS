#ifdef CONFIG_LINUX_COMPAT
.global linux_compat_start
linux_compat_start:
  mov sp, r1
  blx r0
#endif
