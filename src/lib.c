#include "ed.h"

void *handle;

void lib_init(void) {
	handle = dlopen("./libcomm.so", RTLD_LAZY);
}

void lib_term(void) {
	dlclose(handle);
}

void comm_exit(void) {
	lib_term();
	exit(EXIT_SUCCESS);
}

struct parse find_comm(char *inp) {
	struct parse ret;
	ret.cont = inp;
	ret.ok = PARSE_FAIL_GENERAL;

	while (1) {
		if (*ret.cont == '\0') {
			ret.ok = PARSE_UNEXPECTED_NUL;
			break;
		} else if (*ret.cont == '.') {
			ret.cont++;
		} else if (*ret.cont == '$') {
			ret.cont++;
		} else if (isdigit(*ret.cont)) {
			ret.cont++;
		} else if (*ret.cont == '\'') {
			ret.cont++;
			if (islower(*ret.cont)) {
				ret.cont++;
			} else {
				ret.ok = PARSE_INVALID_MARK;
				break;
			}
		} else if (*ret.cont == '/') {
			ret.cont++;
			int esc = 0;
			while (*ret.cont != '\0' && *ret.cont != '\n') {
				if (esc) {
					esc = 0;
					ret.cont++;
				} else {
					if (*ret.cont == '\\') {
						esc = 1;
					} else if (*ret.cont == '/') {
						ret.cont++;
						break;
					}
					ret.cont++;
				}
			}
			if (*ret.cont == '\0') {
				ret.ok = PARSE_UNEXPECTED_NUL;
				break;
			} else if (*ret.cont == '\n') {
				ret.ok = PARSE_OK;
				break;
			}
		} else if (*ret.cont == '?') {
			ret.cont++;
			int esc = 0;
			while (*ret.cont != '\0' && *ret.cont != '\n') {
				if (esc) {
					esc = 0;
					ret.cont++;
				} else {
					if (*ret.cont == '\\') {
						esc = 1;
					} else if (*ret.cont == '?') {
						ret.cont++;
						break;
					}
					ret.cont++;
				}
			}
			if (*ret.cont == '\0') {
				ret.ok = PARSE_UNEXPECTED_NUL;
				break;
			} else if (*ret.cont == '\n') {
				ret.ok = PARSE_OK;
				break;
			}
		} else if (*ret.cont == '+') {
			ret.cont++;
		} else if (*ret.cont == '-') {
			ret.cont++;
		} else if (*ret.cont == ',') {
			ret.cont++;
		} else if (*ret.cont == ';') {
			ret.cont++;
		} else if (isblank(*ret.cont)) {
			ret.cont++;
		} else {
			ret.ok = PARSE_OK;
			break;
		}
	}

	return ret;
}

void load(struct command comm, char *orig, struct std_ed_state *state) { // TODO: support dlerror stuff
	char buff[32];
	snprintf(buff, 32, "commasc_%03i", (int)comm.name);
	void (*func)(struct command comm, char *orig, struct std_ed_state *state) = dlsym(handle, buff);
	func(comm, orig, state); // TODO: support dlerror stuff
}

void set_ed_error(char *s, struct std_ed_state *state) {
	state->ed_error = s;
	puts("?");
	if (state->help_mode) {
		puts(state->ed_error);
	}
}

struct line *buffer_index(struct buffer buff, addr i) {
	if (i < 1) {
		return NULL;
	}
	struct line *p = buff.head;
	for (addr n = 1; n < i; n++) {
		p = p->next;
	}
	return p;
}

addr buffer_find(struct buffer buff, struct line *i) {
	if (i == NULL) {
		return -1;
	}
	struct line *p = buff.head;
	addr n;
	for (n = 1; p != i; n++) {
		p = p->next;
	}
	return n;
}

void buffer_insert_after(struct line *after, struct buffer *in, struct buffer *new) {
	if (new->head == NULL) {
		return;
	}
	struct line *a = after->next;
	after->next = new->head;
	new->head->prev = after;
	if (a != NULL) {
		a->prev = new->tail;
		new->tail->next = a;
	} else {
		in->tail = new->tail;
	}
}

void buffer_insert_before(struct line *before, struct buffer *in, struct buffer *new) {
	if (new->head == NULL) {
		return;
	}
	struct line *a = before->prev;
	before->prev = new->tail;
	new->tail->next = before;
	if (a != NULL) {
		a->next = new->head;
		new->head->prev = a;
	} else {
		in->head = new->head;
	}
}

void buffer_remove(struct buffer *in, struct buffer *old) {
	struct line *a = old->head->prev;
	struct line *b = old->tail->next;
	if (a != NULL) {
		a->next = b;
	} else {
		in->head = b;
	}
	if (b != NULL) {
		b->prev = a;
	} else {
		in->tail = a;
	}
}

void buffer_delete(struct buffer *in, struct buffer *bad) {
	buffer_remove(in, bad);
	struct line *del = bad->head;
	if (del == NULL) {
		return;
	}
	while (del != bad->tail) {
		free(del->text);
		free(del);
		del = del->next;
	}
	bad->head = NULL;
	bad->tail = NULL;
}

struct buffer buffer_dup(struct buffer in) {
	struct buffer ret;
	struct line *write;
	if (in.head == NULL) {
		ret.head = NULL;
		ret.tail = NULL;
		return ret;
	} else {
		write = malloc(sizeof(struct line));
		write->next = NULL;
		write->prev = NULL;
		write->text = strdup(in.head->text);
		ret.head = write;
	}

	struct line *a;
	struct line *read = in.head;
	while (read != NULL) {
		a = write;
		write = malloc(sizeof(struct line));
		write->next = NULL;
		write->prev = a;
		write->text = strdup(read->text);
	}
	
	ret.tail = write;
	return ret;
}

struct buffer buffer_read_file(char *fname) {
	struct buffer ret;
	FILE *file = fopen(fname, "r");
	struct line *write;

	if (!feof(file)) {
		write = malloc(sizeof(struct line));
		write->next = NULL;
		write->prev = NULL;
		char *inp;
		size_t len; // TODO: perhaps do something with this if line gets a len member
		getline(&inp, &len, file);
		write->text = strdup(inp);
		ret.head = write;
	} else {
		ret.head = NULL;
		ret.tail = NULL;
		return ret;
	}

	struct line *a;
	while (!feof(file)) {
		a = write;
		write = malloc(sizeof(struct line));
		write->next = NULL;
		write->prev = a;
		a->next = write;
		char *inp;
		size_t len; // TODO: perhaps do something with this if line gets a len member
		getline(&inp, &len, file);
		write->text = strdup(inp);
	}

	ret.tail = write;
	return ret;
}

struct buffer buffer_read_input(void) {
	struct buffer ret;
	struct line *write;
	char *inp;
	size_t len; // TODO: perhaps do something with this if line gets a len member
	ssize_t len2 = getline(&inp, &len, stdin);

	if (strcmp(inp, ".\n") == 0) {
		ret.head = NULL;
		ret.tail = NULL;
		return ret;
	} else {
		write = malloc(sizeof(struct line));
		ret.head = write;
		write->text = strdup(inp);
		write->prev = NULL;
		write->next = NULL;
	}

	int eof = 0;
	struct line *a;
	while (!eof) {
		ssize_t len2 = getline(&inp, &len, stdin);
		if (strcmp(inp, ".\n") == 0) {
			free(inp);
			eof = 1;
		} else {
			a = write;
			write = malloc(sizeof(struct line));
			write->text = strdup(inp);
			write->prev = a;
			a->next = write;
			write->next = NULL;
		}
	}

	ret.tail = write;
	return ret;
}

// assumptions: cflags has REG_NEWLINE and not REG_NOSUB.
struct line *buffer_search_forward(struct buffer in, struct line *at, regex_t *match) {
	struct line *read;

	if (at->next == NULL) {
		return NULL;
	} else {
		read = at->next;
	}

	regmatch_t captures[10];
	do {
		int x = regexec(match, read->text, 10, captures, 0);
		if (x == 0) {
			return read;
		} else {
			if (read->next == NULL) {
				read = in.head;
			} else {
				read = read->next;
			}
		}
	} while (read != at);

	return NULL;
}

// assumptions: cflags has REG_NEWLINE and not REG_NOSUB.
struct line *buffer_search_backward(struct buffer in, struct line *at, regex_t *match) {
	struct line *read;

	if (at->prev == NULL) {
		return NULL;
	} else {
		read = at->prev;
	}

	regmatch_t captures[10];
	do {
		int x = regexec(match, read->text, 10, captures, 0);
		if (x == 0) {
			return read;
		} else {
			if (read->prev == NULL) {
				read = in.tail;
			} else {
				read = read->prev;
			}
		}
	} while (read != at);

	return NULL;
}

struct parse_addr parse_one_address(char *inp, addr start, struct std_ed_state *state) {
	struct parse_addr ret;
	ret.ok = PARSE_FAIL_GENERAL;
	ret.cont = inp;
	ret.d = start;

	while (1) {
		if (*ret.cont == '\0') {
			ret.ok = PARSE_OK;
			return ret;
		} else if (*ret.cont == '.') {
			ret.d = state->current_addr;
			ret.cont++;
		} else if (*ret.cont == '$') {
			ret.d = buffer_find(state->current_buffer, state->current_buffer.tail);
			ret.cont++;
		} else if (isdigit(*ret.cont)) {
			char *a = ret.cont;
			while (isdigit(*a)) {
				a++;
			}

			char *x = strndup(ret.cont, a - ret.cont);
			if (ret.d == DEF_ADDR) {
				ret.d = 0;
			}
			ret.d += atoi(x);
			ret.cont = a;
			free(x);
		} else if (*ret.cont == '\'') {
			ret.cont++;
			if (islower(*ret.cont)) {
				ret.d = state->marks[*ret.cont - 'a'];
				ret.cont++;
			} else {
				ret.ok = PARSE_INVALID_MARK;
				return ret;
			}
		} else if (*ret.cont == '/') {
			ret.cont++;
			char *a = ret.cont;
			int esc = 0;

			while (*a != '\0' && *a != '\n') {
				if (esc) {
					esc = 0;
					a++;
				} else {
					if (*a == '\\') {
						esc = 1;
					} else if (*a == '/') {
						a++;
						break;
					}
					a++;
				}
			}
			a--;

			char *x = strndup(ret.cont, a - ret.cont);
			regex_t r;
			regcomp(&r, x, 0);
			struct line *here = buffer_index(state->current_buffer, state->current_addr);
			if (here == NULL) {
				ret.ok = PARSE_REGEX_NO_MATCHES;
				return ret;
			}
			struct line *there = buffer_search_forward(state->current_buffer, here, &r);
			ret.d = buffer_find(state->current_buffer, there);
			a++;
			ret.cont = a;
		} else if (*ret.cont == '?') {
			ret.cont++;
			char *a = ret.cont;
			int esc = 0;

			while (*a != '\0' && *a != '\n') {
				if (esc) {
					esc = 0;
					a++;
				} else {
					if (*a == '\\') {
						esc = 1;
					} else if (*a == '?') {
						a++;
						break;
					}
					a++;
				}
			}
			a--;

			char *x = strndup(ret.cont, a - ret.cont);
			regex_t r;
			regcomp(&r, x, 0);
			struct line *here = buffer_index(state->current_buffer, state->current_addr);
			if (here == NULL) {
				ret.ok = PARSE_REGEX_NO_MATCHES;
				return ret;
			}
			struct line *there = buffer_search_backward(state->current_buffer, here, &r);
			ret.d = buffer_find(state->current_buffer, there);
			a++;
			ret.cont = a;
		} else if (*ret.cont == '+') {
			char *a = ret.cont;
			a++;
			while (isblank(*a)) {
				a++;
			}

			if (isdigit(*a)) {
				char *b = a;
				while (isdigit(*b)) {
					b++;
				}
				char *x = strndup(a, b - a);
				if (ret.d == DEF_ADDR) {
					ret.d = state->current_addr;
				}
				ret.d += atoi(x);
				free(x);
			} else {
				if (ret.d == DEF_ADDR) {
					ret.d = state->current_addr;
				}
				ret.d++;
			}

			ret.cont = a;
		} else if (*ret.cont == '-') {
			char *a = ret.cont;
			a++;
			while (isblank(*a)) {
				a++;
			}

			if (isdigit(*a)) {
				char *b = a;
				while (isdigit(*b)) {
					b++;
				}
				char *x = strndup(a, b - a);
				if (ret.d == DEF_ADDR) {
					ret.d = state->current_addr;
				}
				ret.d -= atoi(x);
				free(x);
			} else {
				if (ret.d == DEF_ADDR) {
					ret.d = state->current_addr;
				}
				ret.d--;
			}

			ret.cont = a;
		} else if (*ret.cont == ',') {
			ret.ok = PARSE_OK;
			return ret;
		} else if (*ret.cont == ';') {
			ret.ok = PARSE_OK;
			return ret;
		} else {
			ret.ok = PARSE_OK;
			return ret;
		}
	}
}

// inv or def?
// the command's special case is only if nothing is before the command. otherwise, the normal rules for addresses are used.
// ,, -> 1,$,$ -> $,$ and the left of ,; is . (separator override?)
// this is w.i.p.
// parse first address, then do while loop? _*,_;-:_ -> 1,_*;-:, -...> 1,$;$-1*:;
struct parse_addrr parse_two_address_fake(char *inp, struct std_ed_state *state) {
	struct parse_addrr ret;
	struct parse_addr x = parse_one_address(inp, DEF_ADDR, state);
	ret.d.start = x.d;
	ret.semi = *(++x.cont) == ';';
	ret.d.end = parse_one_address(x.cont, DEF_ADDR, state).d;
	ret.on = 2;
	ret.ok = x.ok;
	return ret;
}
struct parse_addrr parse_two_address(char *inp, struct std_ed_state *state) {

	return parse_two_address_fake(inp, state);

	struct parse_addrr ret;
	
	ret.ok = PARSE_FAIL_GENERAL;
	ret.cont = inp;
	ret.d.start = INV_ADDR;
	ret.d.end = INV_ADDR;
	ret.on = 0;
	ret.semi = -1;

	if (*parse_one_address(inp, DEF_ADDR, state).cont == '\0') {
		ret.d.start = DEF_ADDR;
		ret.d.end = DEF_ADDR;
		ret.ok = PARSE_OK;
		return ret;
	}

	addr at = DEF_ADDR;
	while (1) {
		struct parse_addr this = parse_one_address(ret.cont, at, state);
		if (this.ok != PARSE_OK) {
			ret.ok = this.ok;
			return ret;
		}

		ret.cont = this.cont;
		if (ret.on == 0) {
			ret.d.start = this.d;
			ret.on = 1;
		} else if (ret.on == 1) {
			ret.d.end = this.d;
			ret.on = 2;
		} else {
			ret.d.start = ret.d.end;
			ret.d.end = this.d;
		}

		if (*ret.cont == ',') {
			ret.semi = 0;
		} else if (*ret.cont == ';') {
			ret.semi = 1;
			if (ret.on == 0) {
				state->current_addr = ret.d.start;
				at = ret.d.start;
			} else {
				state->current_addr = ret.d.end;
				at = ret.d.end;
			}
		} else {
			ret.ok = PARSE_OK;
			return ret;
		}
		ret.cont++;
	}

	

	return ret;
}

void prepare_undo(struct std_ed_state *state) {
	buffer_delete(&state->undo, &state->undo);
	state->undo = buffer_dup(state->current_buffer);
}
