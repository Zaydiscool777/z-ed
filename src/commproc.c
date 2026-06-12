#include <stdio.h>
#include <stdlib.h>
#include "ed.h"

int main() {
	char *inp = NULL;
	size_t len = 0;
	ssize_t ret;

	while (1) {
		ret = getline(&inp, &len, stdin);
		if (ret != -1) {
			printf("valid: %li\n", ret);
			fputs(inp, stdout);
		} else {
			puts("invalid.");
			break;
		}
	}

	free(inp);
	return 1;
}
