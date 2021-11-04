// Copied from: https://github.com/nuta/resea - MIT licensed.
//
// We need this file because some freestanding headers are missing with LLVM
// (the previous build system with gcc was providing this file).
#ifndef STDARG_H
#define STDARG_H

typedef __builtin_va_list va_list;
typedef __builtin_va_list __gnuc_va_list;

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_copy(d, s)  __builtin_va_copy(d, s)

#endif
