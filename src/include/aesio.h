#ifndef HAVE_AESIO
#define HAVE_AESIO

#include <stdio.h>

#include <util.h>

byte *aes256read(FILE *file, size_t *lenret, byte key[32]);
/* The return value of aes256read should be passed to free() when it's done
 * being used. */

int aes256write(byte *ptr, size_t len, FILE *out, byte key[32], FILE *salt);
/* Because of the nature of the salts, there's just no reasonable way to return
 * the amount of data written. What if the encrypted data gets written but not
 * the salt? Because of this, this function just returns non-zero on error. */

#endif
