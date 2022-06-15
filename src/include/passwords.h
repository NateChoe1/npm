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
