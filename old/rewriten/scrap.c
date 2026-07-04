#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <dlfcn.h> //* Much later in development

static int BUFFTEXTSIZE = 1024; //* Unused when it should in bfread_n, but it will soon.

/**
 * @struct line
 * @brief Context and data of a textual line as an element in a doubly linked list.
 * @param text Text in the line, or NULL if there is none.
 * @param next Pointer to the next line, or NULL if there is none.
 * @param prev Pointer to the previous line, or NULL if there is none.
 * @param idx Index of the line in the buffer, or 0 if lacking a proper one.
 * @warning The text must be null-terminated, like the ones in C. This will be fixed in the future.
 */
struct line {
	char *text;
	struct line *next;
	struct line *prev;
	unsigned int idx;
};

/**
 * @brief Attaches the "previous" line pointers in a line structure.
 * @param head The head of the line.
 * @note Is in-place. Thus, void output.
 * @note Using the head of a list of lines is more general than the buffer storing them.
 */
void attach_prevs(struct line *head) {
	struct line *curprev = head;
	if(curprev == NULL) {
		perror("error: attach_prevs recieved was NULL.");
		return; //! early return
	}
	struct line *cur = head->next;
	if(cur == NULL) {
		return; //! early return
	} //? maybe merge into while-loop in optimization period?
	while(cur != NULL) {
		cur->prev = curprev; // by definition (but now defined!)
		curprev = cur; // tommorow's yesterday is...
		cur = cur->next; // current is the next
	}
}

/**
 * @brief Assigns each line in a list an index, as if they were positioned linearly.
 * @param head The head of the linked list of lines, assigned an index of 1.
 * @note Lines are assigned 1-based indices, not 0-based indices.
 */
void assignl_idxs(struct line *head) {
	unsigned int c = 1; // 1-indexed
	struct line *x;
	x = head;
	while(x != NULL) {
		x->idx = c;
		x = x->next;
		c++;
	}
}

/**
 * @struct buffer
 * @brief Context and data of a textual buffer that holds all of its lines.
 * @param head Pointer to the first line in the buffer.
 */
struct buffer {
	struct line *head;
	// List of lines as an array. To use in the future.
	//// struct line *array;
};

/**
 * @brief Gets the line with the given index in a buffer.
 * @param buff The buffer that will be indexed.
 * @param i The index number used to index the buffer.
 * @return A pointer to the line indexed, or NULL if there is none.
 */
struct line *bfgetline(struct buffer buff, unsigned int i) {
	struct line *x;
	x = buff.head;
	while(x->idx != i && x != NULL) {
		x = x->next;
	}
	return x; //* if x is NULL, we will be returning NULL anyway
}

/**
 * @brief Reads a file, allocates a buffer, and writes the file to it naively.
 * @warning This only updates the text and next pointers, but not the previous pointers.
 * @note If the file does not exist, it is created.
 * @param filename A string that is a filename that leads to the file being read.
 * @return A buffer of type `struct buffer*`.
 */
struct buffer *bfread_n(char *filename) {
	FILE *x;
	x = fopen(filename, "r"); // opens file conditionally
	if(x == NULL) { // if file doesn't exist, create it and then open it
		// fclose requires its argument to be non-null //// fclose(x);
		x = fopen(filename, "x+");
		fclose(x);
		x = fopen(filename, "r");
	}

	struct buffer *buff;
	buff = malloc(sizeof(struct buffer)); // new buffer
	if(buff == NULL) {
		perror("error: allocation failed.\n");
		return NULL; //! early return
	}
	buff->head = NULL; // make sure it is empty

	int firstline = 1; // don't link the last line because there is none!
	struct line *l; // the current line

	while(!feof(x)) {
		if(firstline == 1) {
			l = malloc(sizeof(struct line)); // create a new line
			if(l == NULL) {
				perror("error: allocation failed.\n");
				return NULL;
			}
			l->text = NULL; // clear it
			l->next = NULL;
			l->prev = NULL;
			l->idx = 0;
			buff->head = l; // set the buffer's head to it
			firstline = 0; // now there is a previous line to reference
		} else {
			l->next = malloc(sizeof(struct line)); // create the next line
			if(l->next == NULL) {
				perror("error: allocation failed.\n");
				return NULL;
			}
			l = l->next; // go to the next line
			l->text = NULL; // clear it
			l->next = NULL;
			l->prev = NULL;
			l->idx = 0;
		}

		char *btext;
		btext = malloc(sizeof(char) * BUFFTEXTSIZE); // make buffer for line
		if(btext == NULL) {
			perror("error: allocation failed.\n");
		}

		size_t btext_len;
		btext_len = sizeof(char) * 1;
		size_t btext_alen;
		btext_alen = BUFFTEXTSIZE;

		btext[0] = '\0'; // clear
		char c = '\0';
		c = fgetc(x); // get first character to check
		
		//* newline ends line, so it breaks if so later in the loop
		//* in fact, if this one ends by feof, then the other one would end too.
		while(!feof(x)) { //? check for c != 0?

			btext_len += sizeof(char); // increment length

			if(btext_len > btext_alen) {
				btext_alen += sizeof(char) * BUFFTEXTSIZE;
				void *realloc_check; // realloc may return an error
				realloc_check = realloc(btext, btext_alen);

				if(realloc_check == NULL) { // failed
					// TODO: make this print to stderr, add errno, and likely ed's '?' feature
					printf("error: reallocation failed. btext_alen: %lu.\n", btext_alen);
					return NULL; //! early return
				} else { // success
					btext = realloc_check; // the memory can move
				}
			}

			btext[btext_len - 2] = c; // "...c\0"
			btext[btext_len - 1] = '\0'; // len - 1 is the ultimate element

			if(c == '\n') { // newline must be added, so add and THEN end
				break; //! early return
			} // we do not want to leave the first character of the next line
			c = fgetc(x); // get next character
		} // loop back to deal with next character

		l->text = strdup(btext);
		free(btext); //! check if this is safe
		btext_len = 0;
	}
	return buff; //? is there preparation before return? Probably not, since it is naive.
}

/**
 * @brief Truncates a file, and writes a buffer to it naively.
 * @note If the file does not exist, it is created.
 * @param buff A buffer structure that is being written to a file.
 * @param filename A string that is a filename that leads to the file being read.
 */
void bfwrite_n(struct buffer buff, char *filename) {
	FILE *x = fopen(filename, "w");
	if(x == NULL){
		printf("error: could not open file. filename: %s", filename);
		return; //! early return
	}

	struct line *l;
	l = buff.head;
	while(l != NULL) {
		int err; // in case of error
		err = fputs(l->text, x);
		if(err == EOF) {
			printf("error: could not write to file. filename: %s", filename);
			return; //! Early return
		}
		l = l->next;
	}

	fclose(x);
}


int main() {
	struct buffer* x;
	char* f = "tests/file2";
	x = bfread_n(f);
	printf("%sEND\n", x->head->text);
	printf("%sEND\n", x->head->next->text);
	x->head->next->text = strdup("overwrite\n");
	bfwrite_n(*x, f);
}
