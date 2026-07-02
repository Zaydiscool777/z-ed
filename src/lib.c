#include "ed.h"

void *handle;
char *ed_error;

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
			while (isdigit(*ret.cont)) {
				ret.cont++;
			}
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
				ret.ok = PARSE_OK;
				break;
			} else if (*ret.cont == '\n') {
				ret.ok = PARSE_UNEXPECTED_NEWLINE;
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
				ret.ok = PARSE_OK;
				break;
			} else if (*ret.cont == '\n') {
				ret.ok = PARSE_UNEXPECTED_NEWLINE;
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
		} else if (*ret.cont == ' ') {
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

void line_insert_after(struct line *after, struct line *new) {
	if (after->buffer == NULL) {
		after->buffer->head = new;
		new->buffer = after->buffer;
	}
	struct line *a = after->next;
	new->prev = after;
	new->next = a;
	after->next = new;
	if (a != NULL) {
		a->prev = new;
	}
	new->buffer = after->buffer;
}

void line_insert_before(struct line *before, struct line *new) {
	struct line *a = before->prev;
	new->next = before;
	new->prev = a;
	before->prev = new;
	if (a != NULL) {
		a->next = new;
	}
	new->buffer = before->buffer;
}

void line_delete(struct line *old) {
	struct line *a = old->prev;
	struct line *b = old->next;
	if (a != NULL) {
		a->next = b;
	}
	if (b != NULL) {
		b->prev = a;
	}
	old->buffer = NULL;
}

struct buffer buffer_read_file(char *fname) {
	struct buffer ret;
	FILE *file = fopen(fname, "r");
	struct line *write;
	if (!feof(file)) {
		write = malloc(sizeof(struct line));
		write->buffer = &ret;
		write->next = NULL;
		write->prev = NULL;
		size_t len; // TODO: perhaps do something with this if line gets a len member
		getline(&write->text, &len, file);
		ret.head = write;
	} else {
		ret.head = NULL;
		return ret;
	}
	struct line *a;
	while (!feof(file)) {
		a = write;
		write = malloc(sizeof(struct line));
		write->buffer = &ret;
		write->next = NULL;
		write->prev = a;
		a->next = write;
		size_t len; // TODO: perhaps do something with this if line gets a len member
		getline(&write->text, &len, file);
	}
	return ret;
}
