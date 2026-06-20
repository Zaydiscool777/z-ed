#include "ed.h"

int main() {
	char *inp = NULL;
	size_t len = 0;

	ssize_t len2 = getline(&inp, &len, stdin);
	if (len2 == -1) {
		puts("eof");
		return EXIT_FAILURE;
	}

	struct parse ret = find_comm(inp);

	if (ret.ok == 1) {
		puts(ret.cont);
		puts("ok");
		return EXIT_SUCCESS;
	} else {
		puts("not ok");
		return EXIT_FAILURE;
	}

}
