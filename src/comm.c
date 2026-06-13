#include "ed.h"

void d(char *inp) {
	puts("d");
}

void c(char *inp) {
	inp[1] = 'c';
	puts(inp + 1); // two newlines
}
