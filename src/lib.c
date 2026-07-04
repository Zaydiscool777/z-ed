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

// TODO: since lines could be at head or tail, add buffer of line as an argument, so that its head and tail can also be changed
void buffer_insert_after(struct line *after, struct buffer *new) {
	struct line *a = after->next;
	if (new->head == NULL) {
		return;
	}
	after->next = new->head;
	new->head->prev = after;
	if (a != NULL) {
		a->prev = new->tail;
		new->tail->next = a;
	}
}

// TODO: since lines could be at head or tail, add buffer of line as an argument, so that its head and tail can also be changed
void buffer_insert_before(struct line *before, struct buffer *new) {
	struct line *a = before->prev;
	if (new->head == NULL) {
		return;
	}
	before->prev = new->tail;
	new->tail->next = before;
	if (a != NULL) {
		a->next = new->head;
		new->head->prev = a;
	}
}

// TODO: since lines could be at head or tail, add buffer of line as an argument, so that its head and tail can also be changed
void buffer_remove(struct buffer *old) {
	struct line *a = old->head->prev;
	struct line *b = old->tail->next;
	if (a != NULL) {
		a->next = b;
	}
	if (b != NULL) {
		b->prev = a;
	}
}

// TODO: since lines could be at head or tail, add buffer of line as an argument, so that its head and tail can also be changed
void buffer_delete(struct buffer *bad) {
	buffer_remove(bad);
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
		write->text = inp;
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
		write->text = inp;
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
		write->text = inp;
		write->prev = NULL;
		write->next = NULL;
	}
	int eof = 0;
	struct line *a;
	while (!eof) {
		ssize_t len2 = getline(&inp, &len, stdin);
		if (strcmp(inp, ".\n") == 0) {
			return;
		} else {
			a = write;
			write = malloc(sizeof(struct line));
			write->text = inp;
			write->prev = a;
			a->next = write;
			write->next = NULL;
		}
	}
	ret.tail = write;
	return ret;
}
