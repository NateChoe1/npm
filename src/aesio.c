/*
 * NPM - Nate's Password Manager
 * Copyright (C) 2022  Nate Choe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * */

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
