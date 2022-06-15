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
