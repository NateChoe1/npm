#ifndef HAVE_UTIL
#define HAVE_UTIL

#include <stddef.h>
#include <limits.h>

typedef unsigned char byte;
typedef unsigned long u32;

struct binstring {
	size_t len;
	size_t alloc;
	byte *data;
};

void *xmalloc(size_t len);
void *xrealloc(void *ptr, size_t newlen);
char *concat(char *str1, ... /*, NULL */);
struct binstring *newbinstring();
void addcbinstring(struct binstring *binstring, byte b);
void addu32binstring(struct binstring *binstring, u32 n);
void addstrbinstring(struct binstring *binstring, char *s);

#define RTR(v, shf) (((v >> shf) | (v << (32 - shf))) & 0xffffffff)
#define RTL(v, shf) (((v << shf) | (v >> (32 - shf))) & 0xffffffff)
#define LEN(list) (sizeof(list) / sizeof(list[0]))

#endif
