#include "ed.h"

int main() {
	lib_init();

	char *inp = NULL;
	size_t len = 0;

	ssize_t len2 = getline(&inp, &len, stdin);
	if (len2 == -1) {
		puts("eof");
		return EXIT_FAILURE;
	}

	struct parse ret = find_comm(inp);

	if (ret.ok == 0) {
		puts("not ok");
		return EXIT_FAILURE;
	}
	
	struct command comm;
	comm.args = calloc(abs(ret.cont - inp) + 1, sizeof(char));
	strncpy(comm.args, ret.cont, abs(ret.cont - inp));
	comm.name = *ret.cont;
	comm.args = ret.cont + 1;
	load(comm);

	lib_term();
	return EXIT_SUCCESS;

}
