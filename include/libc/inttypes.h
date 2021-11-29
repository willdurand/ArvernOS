#ifndef INTTYPES_H
#define INTTYPES_H

#ifdef __arm__
#define PRI_PREFIX "l"
#else
#define PRI_PREFIX "ll"
#endif

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
