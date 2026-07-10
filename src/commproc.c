#include "ed.h"

int main() {
	lib_init();

	current_addr = 1;

	char *inp = NULL;
	size_t len = 0;

	while (1) {
		ssize_t len2 = getline(&inp, &len, stdin);
		if (len2 == -1) {
			puts("eof");
			break;
		}

		struct parse ret = find_comm(inp);

		if (ret.ok == PARSE_OK) {
			struct command comm;
			comm.name = *ret.cont;
			comm.args = ret.cont + 1;
			char *s = strndup(inp, ret.cont - inp);
			struct parse_addrr x = parse_two_address(s);
			comm.range = x.d;

			load(comm); // TODO: support dlerror stuff
		} else {
			set_ed_error("Invalid ...");
		}

		free(inp);
		inp = NULL;
		len = 0;
	}

	free(inp);
	lib_term();
	return EXIT_SUCCESS;
}
