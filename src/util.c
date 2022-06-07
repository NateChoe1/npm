#include <stdio.h>
#include <stdlib.h>

#include <util.h>

void *xmalloc(size_t len) {
	void *ret;
	ret = malloc(len);
	if (ret == NULL) {
		fputs("xmalloc() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ret;
}

void *xrealloc(void *ptr, size_t newlen) {
	void *ret;
	ret = realloc(ptr, newlen);
	if (ret == NULL) {
		fputs("xrealloc() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ret;
}
