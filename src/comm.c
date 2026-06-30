#include "ed.h"

void commasc_100(struct command comm) {
	puts("d");
}

void commasc_099(struct command comm) {
	putc('c', stdout);
	puts(comm.args + 1); // two newlines
}
