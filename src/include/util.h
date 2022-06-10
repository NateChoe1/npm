#ifndef HAVE_UTIL
#define HAVE_UTIL

#include <stddef.h>
#include <limits.h>

typedef unsigned char byte;
typedef unsigned long u32;

void *xmalloc(size_t len);
void *xrealloc(void *ptr, size_t newlen);

#define RTR(v, shf) (((v >> shf) | (v << (32 - shf))) & 0xffffffff)
#define RTL(v, shf) (((v << shf) | (v >> (32 - shf))) & 0xffffffff)
#define LEN(list) (sizeof(list) / sizeof(list[0]))

#endif
