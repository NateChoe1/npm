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

#include <util.h>
#include <passwords.h>

static void addproperty( size_t *len, size_t *alloc,
		struct property **properties, char *name, char *value);
static void addparse(struct password *pass, char *property,
		void (*addpass)(struct password *pass,
			char *name, char *value));

struct password *newpassword() {
	struct password *ret;
	ret = xmalloc(sizeof *ret);
	ret->publiclen = 0;
	ret->publicalloc = 5;
	ret->public = xmalloc(ret->publiclen * sizeof *ret->public);
	ret->privatelen = 0;
	ret->privatealloc = 5;
	ret->private = xmalloc(ret->privatelen * sizeof *ret->private);
	return ret;
}

void freepassword(struct password *password) {
	int i;
	for (i = 0; i < password->publiclen; ++i) {
		free(password->public[i].name);
		free(password->public[i].value);
	}
	free(password->public);
	for (i = 0; i < password->privatelen; ++i) {
		free(password->private[i].name);
		free(password->private[i].value);
	}
	free(password->private);
	free(password);
}

void addpublic(struct password *pass, char *name, char *value) {
	addproperty(&pass->publiclen, &pass->publicalloc, &pass->public,
			name, value);
}

void addprivate(struct password *pass, char *name, char *value) {
	addproperty(&pass->privatelen, &pass->privatealloc, &pass->private,
			name, value);
}

void addpublicparse(struct password *pass, char *property) {
	addparse(pass, property, addpublic);
}

void addprivateparse(struct password *pass, char *property) {
	addparse(pass, property, addprivate);
}

int issubset(const struct password *small, const struct password *big) {
	int i, j;
	for (i = 0; i < small->publiclen; ++i) {
		const struct property *smallprop = small->public + i;
		for (j = 0; j < big->publiclen; ++j) {
			const struct property *bigprop = big->public + j;
			if (strcmp(smallprop->name, bigprop->name) == 0 &&
				strcmp(smallprop->value, bigprop->value) == 0)
				goto foundmatch;
		}
		return 0;
foundmatch:;
	}
	return 1;
}

static void addproperty(size_t *len, size_t *alloc,
		struct property **properties, char *name, char *value) {
	if (*len >= *alloc) {
		*alloc *= 2;
		*properties = xrealloc(*properties,
				*alloc * sizeof **properties);
	}
	(*properties)[*len].name = name;
	(*properties)[*len].value = value;
	++*len;
}

static void addparse(struct password *pass, char *property,
		void (*addpass)(struct password *pass,
			char *name, char *value)) {
	char *name, *value;
	size_t split, len;

	split = -1;
	for (len = 0; property[len] != '\0'; ++len)
		if (split == -1 && property[len] == ':')
			split = len;

	name = malloc(split + 1);
	memcpy(name, property, split);
	name[split] = '\0';
	value = malloc(len - split);
	memcpy(value, property + split + 1, len - split);

	addpass(pass, name, value);
}
