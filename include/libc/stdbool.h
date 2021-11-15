// Based on musl libc (Rich Felker, et al). MIT licensed.
// Copied from: https://github.com/nuta/resea - MIT licensed.
//
// We need this file because some freestanding headers are missing with LLVM
// (the previous build system with gcc was providing this file).
#ifndef STDBOOL_H
#define STDBOOL_H

#define true 1
#define false 0
#define bool _Bool

#endif
