#ifndef HAVE_HASH
#define HAVE_HASH

#include <stddef.h>

typedef unsigned char byte;

void sha256(byte ret[32], const byte *data, size_t len);

#endif
