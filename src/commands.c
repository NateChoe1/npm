#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <util.h>
#include <hash.h>
#include <aesio.h>
#include <commands.h>

static char *getdefpasspath();
static char *concat(char *str1, ... /*, NULL */);

int addpass(int argc, char **argv) {
	char *data = "lasjdfkashfjkhasdjkf";
	size_t len = strlen(data);
	FILE *out = fopen(argv[1], "w");
	FILE *salt = fopen("/dev/urandom", "r");
	byte key[32];

	sha256(key, (byte *) argv[2], strlen(argv[2]));

	aes256write((byte *) data, len, out, key, salt);
	return 0;
}

int updatepass(int argc, char **argv) {
	byte *data;
	byte key[32];
	FILE *file = fopen(argv[1], "r");
	size_t len;

	sha256(key, (byte *) argv[2], strlen(argv[2]));

	data = aes256read(file, &len, key);
	fwrite(data, 1, len, stdout);
	return 0;
}

int getpass(int argc, char **argv) {
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

static char *concat(char *str1, ...) {
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
