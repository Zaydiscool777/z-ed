#include "ed.h"

struct buffer reading;

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

void commasc_101(struct command comm) {
	comm.args[strlen(comm.args) - 1] = '\0';
	reading = buffer_read_file(comm.args);
}

void commasc_097(struct command comm) {
	reading = buffer_read_input();
}

void commasc_112(struct command comm) {
	if (reading.head == NULL) {
		set_ed_error("Invalid address (eof)");
	} else {
		fputs(reading.head->text, stdout);
		reading.head = reading.head->next;
	}
}
