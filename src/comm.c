#include "ed.h"

// '!'
void commasc_033(struct command comm, char *orig, struct std_ed_state *state) {
	pid_t exec = fork();
	if (exec == 0) {
		execlp("sh", "sh", "-c", comm.args, NULL);
	} else {
		int x;
		waitpid(exec, &x, 0);
		puts("!");
	}
}

// '#'
void commasc_023(struct command comm, char *orig, struct std_ed_state *state) {
	
}

// 'q'
void commasc_113(struct command comm, char *orig, struct std_ed_state *state) {
	if (state->mod_buffer == 1) {
		set_ed_error("Warning: buffer modified", state);
		state->mod_buffer = 0;
	} else {
		comm_exit();
	}
}

// '\n'
void commasc_010(struct command comm, char *orig, struct std_ed_state *state) {
	if (comm.range.d.end == INV_ADDR) {
		set_ed_error("Invalid address", state);
	} else {
		state->current_addr = comm.range.d.end;
	}
}

// 'h'
void commasc_104(struct command comm, char *orig, struct std_ed_state *state) {
	if (state->ed_error != NULL) {
		puts(state->ed_error);
	}
}

// 'e'
void commasc_101(struct command comm, char *orig, struct std_ed_state *state) {
	comm.args[strlen(comm.args) - 1] = '\0';
	state->current_buffer = buffer_read_file(comm.args);
}

// 'a'
void commasc_097(struct command comm, char *orig, struct std_ed_state *state) {
	state->current_buffer = buffer_read_input();
}

// 'p'
void commasc_112(struct command comm, char *orig, struct std_ed_state *state) {
	if (comm.range.d.end == INV_ADDR) {
		set_ed_error("Invalid address", state);
	} else {
		fputs(buffer_index(state->current_buffer, comm.range.d.end)->text, stdout);
	}
}

// '='
void commasc_061(struct command comm, char *orig, struct std_ed_state *state) {
	if (comm.range.d.end == DEF_ADDR) {
		printf("%i\n", comm.range.d.end);
	} else {
		printf("%i\n", buffer_find(state->current_buffer, (state->current_buffer).tail));
	}
}

// 'd'
void commasc_100(struct command comm, char *orig, struct std_ed_state *state) {
	
}
