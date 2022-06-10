#ifndef HAVE_AES
#define HAVE_AES

#include <util.h>

void aes256encrypt(byte ret[16], byte data[16], byte key[32]);
void aes256decrypt(byte ret[16], byte data[16], byte key[32]);
/* if ret == NULL, then data is overwritten. */

#endif
