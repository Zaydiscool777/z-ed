#include "ed.h"

void commasc_113(struct command comm, char *orig, struct std_ed_state *state) {
	if (state->mod_buffer == 1) {
		set_ed_error("Warning: buffer modified", state);
		state->mod_buffer = 0;
	} else {
		comm_exit();
	}
}

void commasc_010(struct command comm, char *orig, struct std_ed_state *state) {
	if (comm.range.end == INV_ADDR) {
		set_ed_error("Invalid address", state);
	} else {
		state->current_addr = comm.range.end;
	}
}

void commasc_104(struct command comm, char *orig, struct std_ed_state *state) {
	if (state->ed_error != NULL) {
		puts(state->ed_error);
	}
}

void commasc_101(struct command comm, char *orig, struct std_ed_state *state) {
	comm.args[strlen(comm.args) - 1] = '\0';
	state->current_buffer = buffer_read_file(comm.args);
}

void commasc_097(struct command comm, char *orig, struct std_ed_state *state) {
	state->current_buffer = buffer_read_input();
}

void commasc_112(struct command comm, char *orig, struct std_ed_state *state) {
	if (comm.range.end == INV_ADDR) {
		set_ed_error("Invalid address", state);
	} else {
		fputs(buffer_index(state->current_buffer, comm.range.end)->text, stdout);
	}
}

void commasc_061(struct command comm, char *orig, struct std_ed_state *state) {
	printf("%i,%i\n", comm.range.start, comm.range.end);
}
