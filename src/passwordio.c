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

#include <stdlib.h>
#include <string.h>

#include <aesio.h>
#include <passwordio.h>

static size_t getpassword(byte *data, struct password **ret);
size_t getproperties(byte *data, struct password *pass,
		void (*addproperty)(struct password *pass,
			char *name, char *value));
static size_t getstring(byte *data, char **ret);
static size_t getu32(byte *data, u32 *ret);
static void addpassbinstring(struct binstring *binstring,
		struct password *pass);
static void addpropbinstring(struct binstring *binstring,
		struct property *prop);

struct file *newfile() {
	struct file *ret;
	ret = xmalloc(sizeof *ret);
	ret->len = 0;
	ret->alloc = 50;
	ret->passwords = malloc(ret->alloc * sizeof *ret->passwords);
	return ret;
}

struct file *readpasswords(FILE *file, byte key[32]) {
	byte *data;
	size_t datalen;
	struct file *ret;
	data = aes256read(file, &datalen, key);
	if (memcmp(data, "NPM\0", 4) != 0) {
		free(data);
		return NULL;
	}
	ret = getpasswords(data, datalen);
	free(data);
	return ret;
}

struct file *getpasswords(byte *data, size_t datalen) {
	size_t len;
	u32 count;
	int i;
	struct file *ret;
	ret = newfile();
	len = 4; /* magic bytes */
	len += getu32(data + len, &count);
	for (i = 0; i < count; ++i) {
		struct password *newpass;
		len += getpassword(data + len, &newpass);
		addpassword(ret, newpass);
	}
	return ret;
}

void freefile(struct file *file) {
	int i;
	for (i = 0; i < file->len; ++i)
		freepassword(file->passwords[i]);
	free(file->passwords);
	free(file);
}

void addpassword(struct file *file, struct password *password) {
	if (file->len >= file->alloc) {
		file->alloc *= 2;
		file->passwords = xrealloc(file->passwords,
				file->alloc * sizeof *file->passwords);
	}
	memcpy(file->passwords + file->len, &password, sizeof password);
	++file->len;
}

int writefile(struct file *file, FILE *out, byte key[32], FILE *salt) {
	struct binstring *data;
	int i;
	data = newbinstring();

	addcbinstring(data, 'N');
	addcbinstring(data, 'P');
	addcbinstring(data, 'M');
	addcbinstring(data, '\0');
	addu32binstring(data, file->len);
	for (i = 0; i < file->len; ++i)
		addpassbinstring(data, file->passwords[i]);
	return aes256write(data->data, data->len, out, key, salt);
}

static size_t getpassword(byte *data, struct password **ret) {
	size_t len;

	*ret = newpassword();

	len = getproperties(data, *ret, addpublic);
	len += getproperties(data + len, *ret, addprivate);

	return len;
}

size_t getproperties(byte *data, struct password *pass,
		void (*addproperty)(struct password *pass,
			char *name, char *value)) {
	size_t len;
	u32 count;
	int i;
	len = getu32(data, &count);
	for (i = 0; i < count; ++i) {
		char *name, *value;
		len += getstring(data + len, &name);
		len += getstring(data + len, &value);
		addproperty(pass, name, value);
	}
	return len;
}

static size_t getstring(byte *data, char **ret) {
	u32 stringlen;
	size_t readlen;
	readlen = getu32(data, &stringlen);
	*ret = malloc(stringlen + 1);
	memcpy(*ret, data + readlen, stringlen + 1);
	return readlen + stringlen + 1;
}

static size_t getu32(byte *data, u32 *ret) {
	int i;
	*ret = 0;
	for (i = 0; i < 4; ++i) {
		*ret <<= 8;
		*ret |= data[i];
	}
	return i;
}

static void addpassbinstring(struct binstring *binstring,
		struct password *pass) {
	int i;
	addu32binstring(binstring, pass->publiclen);
	for (i = 0; i < pass->publiclen; ++i)
		addpropbinstring(binstring, pass->public + i);
	addu32binstring(binstring, pass->privatelen);
	for (i = 0; i < pass->privatelen; ++i)
		addpropbinstring(binstring, pass->private + i);
}

static void addpropbinstring(struct binstring *binstring,
		struct property *prop) {
	addstrbinstring(binstring, prop->name);
	addstrbinstring(binstring, prop->value);
}
