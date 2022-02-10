#ifdef CONFIG_LINUX_COMPAT
.global linux_compat_start
linux_compat_start:
  mov sp, x1
  blr x0
#endif
