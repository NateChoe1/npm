#include <string.h>
#include <stdlib.h>

#include <aes.h>
#include <util.h>
#include <aesio.h>

byte *aes256read(FILE *file, size_t *lenret, byte key[32]) {
	size_t len, alloc;
	byte *ret;
	alloc = 1024;
	ret = xmalloc(alloc);
	len = 0;
	for (;;) {
		byte data[16];
		size_t read;

		read = fread(data, 1, 16, file);
		if (read < 16) {
			if (read == 0 && feof(file)) {
				*lenret = len;
				return ret;
			}
			free(ret);
			return NULL;
		}

		aes256decrypt(NULL, data, key);

		if (len >= alloc) {
			alloc *= 2;
			ret = xrealloc(ret, alloc);
		}

		memcpy(ret + len, data, 8);

		len += 8;
	}
}

int aes256write(byte *ptr, size_t len, FILE *out, byte key[32], FILE *salt) {
	size_t pos;
	byte data[16];
	for (pos = 0; pos < len; pos += 8) {
		memcpy(data, ptr + pos, 8);
		if (fread(data + 8, 1, 8, salt) < 8)
			return 1;
		aes256encrypt(NULL, data, key);
		if (fwrite(data, 1, 16, out) < 16)
			return 1;
	}
	pos -= 8;
	memcpy(data, ptr + pos, len - pos);
	memset(data + (len - pos), 0, 8 - (len - pos));
	if (fread(data + 8, 1, 8, salt) < 8)
		return 1;
	aes256encrypt(NULL, data, key);
	if (fwrite(data, 1, 16, out) < 16)
		return 1;
	return 0;
}
