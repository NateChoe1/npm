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
