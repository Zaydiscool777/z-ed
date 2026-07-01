#include "ed.h"

void commasc_100(struct command comm) {
	puts("d");
}

void commasc_099(struct command comm) {
	putc('c', stdout);
	fputs(comm.args + 1, stdout);
}

void commasc_113(struct command comm) {
	comm_exit();
}

void commasc_010(struct command comm) {
	
}

void commasc_104(struct command comm) {
	puts(ed_error);
}
