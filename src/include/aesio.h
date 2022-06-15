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
