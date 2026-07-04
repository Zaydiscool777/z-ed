#include "../src/ed.h"

void line_insert_after(struct line *after, struct line *new) {
	/* if (after->buffer == NULL) {
		after->buffer->head = new;
		new->buffer = after->buffer;
	} */
	struct line *a = after->next;
	new->prev = after;
	new->next = a;
	after->next = new;
	if (a != NULL) {
		a->prev = new;
	}
	// new->buffer = after->buffer;
}

void line_insert_before(struct line *before, struct line *new) {
	struct line *a = before->prev;
	new->next = before;
	new->prev = a;
	before->prev = new;
	if (a != NULL) {
		a->next = new;
	}
	// new->buffer = before->buffer;
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
	// old->buffer = NULL;
}
