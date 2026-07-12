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

void load(struct command comm) { // TODO: support dlerror stuff
	char buff[32];
	snprintf(buff, 32, "commasc_%03i", (int)(comm.name));
	void (*func)(struct command) = dlsym(handle, buff);
	func(comm); // TODO: support dlerror stuff
}

void set_ed_error(char *s) {
	ed_error = s;
	puts("?");
	if (help_mode) {
		puts(ed_error);
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
// how come forward works normally, but not this?
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

struct parse_addr parse_one_address(char *inp, addr start) {
	struct parse_addr ret;
	ret.ok = PARSE_FAIL_GENERAL;
	ret.cont = inp;
	ret.d = start;

	while (1) {
		if (*ret.cont == '\0') {
			ret.ok = PARSE_OK;
			return ret;
		} else if (*ret.cont == '.') {
			ret.d = current_addr;
			ret.cont++;
		} else if (*ret.cont == '$') {
			ret.d = buffer_find(current_buffer, current_buffer.tail);
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
				ret.d = marks[*ret.cont - 'a'];
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
			struct line *here = buffer_index(current_buffer, current_addr);
			if (here == NULL) {
				ret.ok = PARSE_REGEX_NO_MATCHES;
				return ret;
			}
			struct line *there = buffer_search_forward(current_buffer, here, &r);
			ret.d = buffer_find(current_buffer, there);
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
					if (*a = '\\') {
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
			struct line *here = buffer_index(current_buffer, current_addr);
			if (here == NULL) {
				ret.ok = PARSE_REGEX_NO_MATCHES;
				return ret;
			}
			struct line *there = buffer_search_backward(current_buffer, here, &r);
			ret.d = buffer_find(current_buffer, there);
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
					ret.d = current_addr;
				}
				ret.d += atoi(x);
				free(x);
			} else {
				if (ret.d == DEF_ADDR) {
					ret.d = current_addr;
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
					ret.d = current_addr;
				}
				ret.d -= atoi(x);
				free(x);
			} else {
				if (ret.d == DEF_ADDR) {
					ret.d = current_addr;
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
struct parse_addrr parse_two_address(char *inp) {
	struct parse_addrr ret;
	
	ret.ok = PARSE_FAIL_GENERAL;
	ret.cont = inp;
	ret.d.start = DEF_ADDR;
	ret.d.end = DEF_ADDR;
	ret.on = 0;
	ret.semi = -1;

	if (parse_one_address(inp, DEF_ADDR).cont == inp) {
		ret.ok = PARSE_OK;
		return ret;
	}

	while (1) {
		struct parse_addr x = parse_one_address(ret.cont, DEF_ADDR);
		addr use;
		if (x.ok == PARSE_OK) {
			use = x.d;
		} else {
			ret.ok = x.ok;
			use = INV_ADDR;
		}
		ret.cont = x.cont;
		if (ret.on == 0) {
			ret.d.start = use;
			ret.on = 1;
		} else if (ret.on == 1) {
			ret.d.end = use;
			ret.on = 2;
		} else {
			ret.d.start = ret.d.end;
			ret.d.end = use;
		}
		if (*ret.cont == ',') {
			ret.semi = 0;
			ret.cont++;
		} else if (*ret.cont == ';') {
			ret.semi = 1;
			ret.cont++;
		} else {
			ret.ok = PARSE_OK;
			ret.cont++;
			break;
		}
	}

	return ret;
}
