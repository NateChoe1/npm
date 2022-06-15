#ifndef HAVE_PASSWORDIO
#define HAVE_PASSWORDIO

#include <stdio.h>

#include <util.h>
#include <passwords.h>

struct file {
	size_t len;
	size_t alloc;
	struct password **passwords;
};

struct file *newfile();
struct file *readpasswords(FILE *file, byte key[32]);
/* returns NULL for an invalid password only */
struct file *getpasswords(byte *data, size_t datalen);
void freefile(struct file *file);
void addpassword(struct file *file, struct password *password);
int writefile(struct file *file, FILE *out, byte key[32], FILE *salt);

#endif
