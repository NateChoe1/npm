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

#ifndef HAVE_PASSWORDS
#define HAVE_PASSWORDS

#include <stddef.h>

struct property {
	char *name;
	char *value;
};

struct password {
	struct property *public;
	size_t publiclen;
	size_t publicalloc;
	struct property *private;
	size_t privatelen;
	size_t privatealloc;
};

struct password *newpassword();
void freepassword(struct password *password);
void addpublic(struct password *pass, char *name, char *value);
void addprivate(struct password *pass, char *name, char *value);
void addpublicparse(struct password *pass, char *property);
void addprivateparse(struct password *pass, char *property);
int issubset(const struct password *small, const struct password *big);

#endif
