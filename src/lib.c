#include "ed.h"

void *handle;

void lib_init(void) {
	handle = dlopen("./libcomm.so", RTLD_LAZY);
}

void lib_term(void) {
	dlclose(handle);
}

struct parse find_comm(char *inp) {
	struct parse ret;
	ret.cont = inp;
	ret.ok = 0;
	while (1) {
		if (*ret.cont == '\0') {
			ret.ok = 0;
			break;
		} else if (*ret.cont == '\n') {
			ret.ok = 0;
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
			ret.cont += 2;
		} else if (*ret.cont == '/') {
			ret.cont++;
			int esc = 0;
			while (*ret.cont != '\0' && *ret.cont != '\n') {
				if (esc == 1) {
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
				ret.ok = 1;
				break;
			} else if (*ret.cont == '\n') {
				ret.ok = 1;
				break;
			}
		} else if (*ret.cont == '?') {
			ret.cont++;
			int esc = 0;
			while (*ret.cont != '\0' && *ret.cont != '\n') {
				if (esc == 1) {
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
				ret.ok = 1;
				break;
			} else if (*ret.cont == '\n') {
				ret.ok = 1;
				break;
			}
		}
	}
	return ret;
}

void load(struct command comm) {
	char buff[32];
	snprintf(buff, 32, "commasc_%3i", (int)(comm.name));
	void (*func)(struct command) = dlsym(handle, buff);
	func(comm);
	puts("pretend command execution.");
}
