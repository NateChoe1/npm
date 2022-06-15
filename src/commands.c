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

#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <unistd.h>
#include <termios.h>

#include <util.h>
#include <hash.h>
#include <aesio.h>
#include <commands.h>
#include <passwords.h>
#include <passwordio.h>

static char *getdefpasspath();
static char *askpassword(char *prompt);
static char *getline();
static char *strdup(char *str);

int addpass(int argc, char **argv) {
	char *master, *passpath;
	struct password *newpass;
	byte key[32];
	struct file *keyring;

	newpass = newpassword();
	passpath = master = NULL;

	for (;;) {
		int opt;
		opt = getopt(argc, argv, "m:p:s:f:");
		switch (opt) {
		case -1:
			goto gotargs;
		case 'm':
			master = strdup(optarg);
			break;
		case 'p':
			addpublicparse(newpass, optarg);
			break;
		case 's':
			addprivateparse(newpass, optarg);
			break;
		case 'f':
			passpath = strdup(optarg);
			break;
		}
	}
gotargs:
	if (passpath == NULL)
		passpath = getdefpasspath();
	if (master == NULL)
		master = askpassword("Master password: ");

	sha256(key, (byte *) master, strlen(master));
	{
		FILE *in;
		in = fopen(passpath, "r");
		if (in != NULL) {
			keyring = readpasswords(in, key);
			if (keyring == NULL) {
				fputs("Incorrect master password\n", stderr);
				fclose(in);
				return 1;
			}
			fclose(in);
		}
		else
			keyring = newfile();
	}

	addpassword(keyring, newpass);

	{
		FILE *out, *salt;
		salt = fopen("/dev/urandom", "r");
		if (salt == NULL) {
			fputs("Failed to open file /dev/urandom for reading\n",
					stderr);
			return 1;
		}
		out = fopen(passpath, "w");
		if (out == NULL) {
			fprintf(stderr, "Failed to open file %s for writing\n",
					passpath);
			return 1;
		}
		return writefile(keyring, out, key, salt);
	}
}

int updatepass(int argc, char **argv) {
	char *passpath;
	FILE *file;
	FILE *salt;
	struct file *keyring;
	passpath = NULL;
	salt = fopen("/dev/urandom", "r");
	if (salt == NULL) {
		fputs("Failed to open file /dev/urandom for reading\n", stderr);
		return 1;
	}
	for (;;) {
		int opt;
		opt = getopt(argc, argv, "f:");
		switch (opt) {
		case -1:
			goto gotargs;
		case 'f':
			passpath = strdup(optarg);
			break;
		}
	}
gotargs:
	if (argc - optind < 2) {
		fputs("Expected 2 arguments for the old and new passwords",
				stderr);
		goto error1;
	}
	if (passpath == NULL)
		passpath = getdefpasspath();

	file = fopen(passpath, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file %s for reading\n",
				passpath);
		goto error1;
	}

	{
		byte oldkey[32];
		sha256(oldkey, (byte *) argv[optind], strlen(argv[optind]));
		keyring = readpasswords(file, oldkey);
		if (keyring == NULL) {
			fputs("Incorrect old master password\n", stderr);
			goto error2;
		}
		fclose(file);
	}

	file = fopen(passpath, "w");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file %s for reading\n",
				passpath);
		freefile(keyring);
		goto error1;
	}

	{
		byte newkey[32];
		sha256(newkey, (byte *) argv[optind+1], strlen(argv[optind+1]));
		writefile(keyring, file, newkey, salt);
	}

	fclose(file);
	free(passpath);
	freefile(keyring);
	return 0;
error2:
	fclose(file);
error1:
	free(passpath);
	return 1;
}

int getpass(int argc, char **argv) {
	char *master, *passpath;
	byte key[32];
	struct file *keyring;
	int i;

	passpath = master = NULL;

	for (;;) {
		int opt;
		opt = getopt(argc, argv, "m:f:");
		switch (opt) {
		case -1:
			goto gotargs;
		case 'm':
			master = strdup(optarg);
			break;
		case 'f':
			passpath = strdup(optarg);
			break;
		}
	}
gotargs:
	if (passpath == NULL)
		passpath = getdefpasspath();
	if (master == NULL)
		master = askpassword("Master password: ");

	sha256(key, (byte *) master, strlen(master));
	{
		FILE *in;

		in = fopen(passpath, "r");
		if (in == NULL) {
			fprintf(stderr, "Couldn't open file %s for reading\n",
					passpath);
			return 1;
		}

		keyring = readpasswords(in, key);
		if (keyring == NULL) {
			fputs("Incorrect master password\n", stderr);
			return 1;
		}
		fclose(in);
	}

	for (i = 0; i < keyring->len; ++i) {
		int j;
		struct password *pass;
		pass = keyring->passwords[i];
		puts("PUBLIC PROPERTIES:");
		for (j = 0; j < pass->publiclen; ++j)
			printf("%s %s\n", pass->public[j].name, pass->public[j].value);
		putchar('\n');
		puts("PRIVATE PROPERTIES:");
		for (j = 0; j < pass->privatelen; ++j) {
			printf("%s %s\n", pass->private[j].name, pass->private[j].value);
		}
		putchar('\n');
	}
	return 0;
}

int delpass(int argc, char **argv) {
	char *master, *passpath;
	struct password *passmask;
	byte key[32];
	struct file *keyring;
	int i;

	passmask = newpassword();
	passpath = master = NULL;

	for (;;) {
		int opt;
		opt = getopt(argc, argv, "m:p:f:");
		switch (opt) {
		case -1:
			goto gotargs;
		case 'm':
			master = strdup(optarg);
			break;
		case 'p':
			addpublicparse(passmask, optarg);
			break;
		case 'f':
			passpath = strdup(optarg);
			break;
		}
	}
gotargs:
	if (passpath == NULL)
		passpath = getdefpasspath();
	if (master == NULL)
		master = askpassword("Master password: ");

	sha256(key, (byte *) master, strlen(master));
	{
		FILE *in;
		in = fopen(passpath, "r");
		if (in != NULL) {
			keyring = readpasswords(in, key);
			if (keyring == NULL) {
				fputs("Incorrect master password\n", stderr);
				fclose(in);
				return 1;
			}
			fclose(in);
		}
		else
			keyring = newfile();
	}

	for (i = keyring->len - 1; i >= 0; --i) {
		if (issubset(passmask, keyring->passwords[i])) {
			memmove(keyring->passwords + i + 1,
				keyring->passwords + i,
				(keyring->len - i - 1) *
					sizeof *keyring->passwords);
			--keyring->len;
		}
	}

	{
		FILE *out, *salt;
		salt = fopen("/dev/urandom", "r");
		if (salt == NULL) {
			fputs("Failed to open file /dev/urandom for reading\n",
					stderr);
			return 1;
		}
		out = fopen(passpath, "w");
		if (out == NULL) {
			fprintf(stderr, "Failed to open file %s for writing\n",
					passpath);
			return 1;
		}
		return writefile(keyring, out, key, salt);
	}
}

static char *getdefpasspath() {
	if (getenv("XDG_DATA_HOME") != NULL)
		return concat(getenv("XDG_DATA_HOME"), "npm", NULL);
	if (getenv("HOME") != NULL)
		return concat(getenv("HOME"), "/.local/share/npm", NULL);
	return NULL;
}


static char *askpassword(char *prompt) {
	struct termios term;
	char *ret;
	fputs(prompt, stderr);

	tcgetattr(0, &term);
	term.c_lflag &= ~ECHO;
	tcsetattr(0, TCSADRAIN, &term);
	ret = getline();
	term.c_lflag |= ECHO;
	tcsetattr(0, TCSADRAIN, &term);
	putchar('\n');
	return ret;
}

static char *getline() {
	size_t len, alloc;
	char *ret;
	alloc = 20;
	ret = xmalloc(alloc);
	for (len = 0;; ++len) {
		int c;
		if (len >= alloc)
			ret = xrealloc(ret, alloc);
		c = fgetc(stdin);
		if (c == '\n')
			break;
		ret[len] = c;
	}
	ret[len] = '\0';
	return ret;
}

static char *strdup(char *str) {
	size_t len;
	char *ret;
	len = strlen(str) + 1;
	ret = malloc(len);
	memcpy(ret, str, len);
	return ret;
}
