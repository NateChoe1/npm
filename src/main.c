#include <stdio.h>
#include <string.h>

#include <commands.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [add/update/get/del]\n", argv[0]);
		return 1;
	}
	if (strcmp(argv[1], "add") == 0)
		return addpass(argc - 1, argv + 1);
	if (strcmp(argv[1], "update") == 0)
		return updatepass(argc - 1, argv + 1);
	if (strcmp(argv[1], "get") == 0)
		return getpass(argc - 1, argv + 1);
	if (strcmp(argv[1], "del") == 0)
		return delpass(argc - 1, argv + 1);
	fprintf(stderr, "Invalid command %s\n", argv[1]);
	return 1;
}

