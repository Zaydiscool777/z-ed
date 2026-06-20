#include "ed.h"

int main() {
	char *inp = NULL;
	size_t len = 0;
	char funcname[2] = "\0\0";
	void (*func)(struct command);

	while (1) {
		ssize_t ret = getline(&inp, &len, stdin);
		if (ret == -1) {
			puts("eof.");
			break;
		}
		printf("ok: %li\n", ret);
		struct command comm = parse_command(inp);
		load(comm);
	}

	return EXIT_SUCCESS;
}
