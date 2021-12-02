# Component: lib(k|c)

This directory contains both the kernel C library (_libk_) and the standard C
library (_libc_), which share pretty much the same implementation. Some
Makefile-fu and C macros help separate the two libraries.

When adding new code, the general idea is to write a unified implementation,
from a standard C library perspective, and that should be enough to work in the
kernel. That being said:

- if a function requires a different implementation in the kernel, we can use
  directives for conditional compilation:

      #ifdef __is_libk
      // libk code
      #else
      // libc code
      #endif

- if a function only exists in the standard C library (_libc_), it should be
  excluded from the _libk_ entirely with directives. This creates empty modules
  during compilation (and some compilers don't like that) but that is an
  acceptable (and accepted) trade-off at the moment.

      #ifndef __is_libk
      // libc only code
      #endif
