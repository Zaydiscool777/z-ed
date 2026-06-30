#include "ed.h"

void *handle;
char *ed_error;

void lib_init(void) {
	handle = dlopen("./libcomm.so", RTLD_LAZY);
	signal(SIGINT, sigint_handler);
}

void sigint_handler(int signal) {
	lib_term();
}

void lib_term(void) {
	dlclose(handle);
	exit(0);
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
		} else {
			ret.ok = PARSE_OK;
			break;
		}
	}
	return ret;
}

void set_ed_error(char *s) {
	ed_error = s;
	puts("?");
}

void load(struct command comm) {
	char buff[32];
	snprintf(buff, 32, "commasc_%03i", (int)(comm.name));
	void (*func)(struct command) = dlsym(handle, buff);
	func(comm);
}
