#include "ed.h"

int main() {
	void *handle = dlopen("./libcomm.so", RTLD_NOW);
	if (handle == NULL) {
		exit(EXIT_FAILURE);
	}

	char *inp = NULL;
	size_t len = 0;
	char funcname[2] = "\0\0";
	void (*func)();

	while (1) {
		ssize_t ret = getline(&inp, &len, stdin);
		if (ret == -1) {
			puts("eof.");
			break;
		}
		printf("ok: %li\n", ret);
		func = *load(parse_command(inp));
		func();
	}

	free(inp);
	return EXIT_SUCCESS;
}
