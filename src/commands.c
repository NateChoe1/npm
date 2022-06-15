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
			byte *data;
			size_t datalen;
			data = aes256read(in, &datalen, key);
			fclose(in);
			if (memcmp(data, "NPM\0", 4) != 0) {
				fputs("Incorrect master password\n", stderr);
				return 1;
			}
			keyring = readpasswords(data, datalen);
			free(data);
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
	return 0;
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
		byte *data;
		size_t datalen;

		in = fopen(passpath, "r");
		if (in == NULL) {
			fprintf(stderr, "Couldn't open file %s for reading\n",
					passpath);
			return 1;
		}

		data = aes256read(in, &datalen, key);
		fclose(in);
		if (memcmp(data, "NPM\0", 4) != 0) {
			fputs("Incorrect master password\n", stderr);
			return 1;
		}
		keyring = readpasswords(data, datalen);
		free(data);
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
	return 0;
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
