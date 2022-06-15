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
	if (strcmp(argv[1], "legal") == 0) {
		puts(
"NPM - Nate's Password Manager\n"
"This program is free software: you can redistribute it and/or modify it\n"
"under the terms of the GNU General Public Licecnse as published by the Free\n"
"Software Foundation, either version 3 of the License, or (at your option)\n"
"later version.\n\n"

"This program is purely an exercise in encryption. It is distributed in the\n"
"hope that it is helpful, but is NOT DESIGNED FOR PRODUCTIVE USE and comes\n"
"with ABSOLUTELY NO WARRANTY."
		);
	}
	fprintf(stderr, "Invalid command %s\n", argv[1]);
	return 1;
}

