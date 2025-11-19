#include "main.h"

unsigned int parse_uint(char *s){
	int x = 0;
	char *i = s;
	while(*i >= '0' && *i <= '9') {
		x = x * 10 + (*i - '0');
	}
	return x;
}

struct parse_addr interp_addr(struct buffer buff, addr cur, char *s) {
	char *x;
	x = s;
	int c = 0;
	while(*x != NULL) {
		if(*x == '/') {
			char r = 0;
			while(!(*x == '/' && r == 0)) {
				if(*x == '\\') {
					r = 1;
				} else {
					r = 0;
				}
				c++;
				x++;
			}
			// TODO: finish...
		} else if (*x == '?') {
			char r = 0;
			while(!(*x == '?' && r == 0)) {
				if(*x == '\\') {
					r = 1;
				} else {
					r = 0;
				}
				c++;
				x++;
			}
			// TODO: finish...
		} else {
			while(*x >= '0' || *x <= '9'
				|| *x == '+' || *x == '-'
				|| *x == '.' || *x == '$') {
					c++;
					x++;
					// TODO: finish...
			}
		}
	}
}

