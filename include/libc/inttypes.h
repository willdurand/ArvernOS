/** @file */
#ifndef INTTYPES_H
#define INTTYPES_H

#if __WORDSIZE == 64
#define PRI_PREFIX "l"
#else
#define PRI_PREFIX "ll"
#endif

#define PRIu32 "lu"

#ifndef PRIi64
#define PRIi64 PRI_PREFIX "i"
#endif

#ifndef PRIu64
#define PRIu64 PRI_PREFIX "u"
#endif

#ifndef PRIx64
#define PRIx64 PRI_PREFIX "x"
#endif

#ifndef PRId64
#define PRId64 PRI_PREFIX "d"
#endif

#endif
