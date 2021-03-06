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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <util.h>

void *xmalloc(size_t len) {
	void *ret;
	ret = malloc(len);
	if (ret == NULL) {
		fputs("xmalloc() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ret;
}

void *xrealloc(void *ptr, size_t newlen) {
	void *ret;
	ret = realloc(ptr, newlen);
	if (ret == NULL) {
		fputs("xrealloc() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ret;
}

char *concat(char *str1, ...) {
	va_list ap;
	char *ret;
	size_t len;
	va_start(ap, str1);

	len = strlen(str1);
	ret = xmalloc(len + 1);
	memcpy(ret, str1, len);

	for (;;) {
		char *newstring;
		size_t newlen;
		newstring = va_arg(ap, char *);
		if (newstring == NULL)
			break;
		newlen = strlen(newstring);
		ret = xrealloc(ret, len + newlen + 1);
		memcpy(ret + len, newstring, newlen);
		len += newlen;
	}

	ret[len] = '\0';
	return ret;
}

struct binstring *newbinstring() {
	struct binstring *ret;
	ret = xmalloc(sizeof *ret);
	ret->len = 0;
	ret->alloc = 200;
	ret->data = xmalloc(ret->alloc);
	return ret;
}

void addcbinstring(struct binstring *binstring, byte b) {
	if (binstring->len >= binstring->alloc) {
		binstring->alloc *= 2;
		binstring->data = xrealloc(binstring->data, binstring->alloc);
	}
	binstring->data[binstring->len++] = b;
}

void addu32binstring(struct binstring *binstring, u32 n) {
	int i;
	for (i = 24; i >= 0; i -= 8)
		addcbinstring(binstring, (n >> i) & 0xff);
}

void addstrbinstring(struct binstring *binstring, char *s) {
	size_t len;
	int i;
	len = strlen(s);
	addu32binstring(binstring, len);
	for (i = 0; i <= len; ++i)
		addcbinstring(binstring, s[i]);
	/* We use <= because we want to include the '\0' */
}
