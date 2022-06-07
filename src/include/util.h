#ifndef HAVE_UTIL
#define HAVE_UTIL

#include <stddef.h>

void *xmalloc(size_t len);
void *xrealloc(void *ptr, size_t newlen);

#define RTR(v, shf) (((v >> shf) | (v << (32 - shf))) & 0xffffffff)
#define RTL(v, shf) (((v << shf) | (v >> (32 - shf))) & 0xffffffff)

#endif
