#include "ed.h"

struct command parse_command(char *inp) {
	struct command x;
	x.name = inp[0];
	x.range.start = 0;
	x.range.end = 0;
	x.args = "";
	return x;
}

void do_nothing(void) {
	puts("pretend command execution.");
}

void (*load(struct command comm))(/* TODO: put inputs in here */) {
	printf("pretend loading. command: %c\n", comm.name);
	return *do_nothing;
}
