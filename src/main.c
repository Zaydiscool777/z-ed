#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFTEXTSIZE 1024 //* Unused when it should in bfread_n, but it will soon.

/**
 * @struct line
 * @brief Context and data of a textual line as an element in a doubly linked list.
 * @param text Text in the line.
 * @param next Pointer to the next line.
 * @param prev Pointer to the previous line.
 * @param idx Index of the line in the buffer. To use in the future.
 */
struct line {
	char *text;
	struct line *next;
	struct line *prev;
	// unsigned int idx;
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
		// TODO: proper error handling, or add non-null __attribute
		printf("error: attach_prevs recieved was NULL.");
		return; //! Early return
	}
	struct line *cur = head->next;
	if(cur == NULL) {
		return; //! Early return
	} //? Maybe merge into while-loop in optimization period?
	while(cur != NULL) {
		cur->prev = curprev; // by definition (but now defined!)
		curprev = cur; // tommorow's yesterday...
		cur = cur->next; // current is the next
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
 * @brief Reads a file, allocates a buffer, and writes the file to it naively.
 * @warning This only updates the text and next pointers, but not the previous pointers.
 * @note If the file does not exist, it is created.
 * @param filename A string that is a filename that leads to the file being read.
 * @return A buffer of type `struct buffer*`.
 */
struct buffer *bfread_n(char *filename) {
	FILE *x = fopen(filename, "r"); // opens file conditionally
	if(x == NULL) { // if file doesn't exist, create it and then open it
		// fclose requires its argument to be non-null //// fclose(x);
		x = fopen(filename, "x+");
		fclose(x);
		x = fopen(filename, "r");
	}
	struct buffer *buff = malloc(sizeof(struct buffer)); // new buffer
	buff->head = NULL; // make sure it is empty
	int firstline = 1; // don't link the last line because there is none!
	struct line *l; // the current line
	while(!feof(x)) {
		if(firstline == 1) {
			l = malloc(sizeof(struct line)); // create a new line
			l->text = NULL; // clear it
			l->next = NULL;
			l->prev = NULL;
			buff->head = l; // set the buffer's head to it
			firstline = 0; // now there is a previous line to reference
		} else {
			l->next = malloc(sizeof(struct line)); // create the next line
			l = l->next; // go to the next line
			l->text = NULL; // clear it
			l->next = NULL;
			l->prev = NULL;
		}
		char *btext = malloc(sizeof(char) * 1); // make buffer for line
		size_t btext_len = sizeof(char) * 1;
		btext[0] = '\0'; // clear
		char c = '\0';
		c = fgetc(x); // get first character to check
		int on_newline = 0; // newline ends line
		while(!on_newline && !feof(x)) { //? Check for c != 0?
			btext_len += sizeof(char); // increment length
			// TODO: make this increment in sizes of BUFFTEXTSIZE. may require two lengths.
			void *realloc_check; // realloc may return an error
			realloc_check = realloc(btext, btext_len);
			if(realloc_check == NULL) { // failed
				// TODO: add errno, and likely ed's '?' feature
				printf("error: reallocation failed. btext_len: %lu.\n", btext_len);
			} else { // success
				btext[btext_len - 2] = c; // "...c\0"
				btext[btext_len - 1] = '\0'; // len - 1 is the ultimate element
				if(c == '\n') { // newline must be added, so check for newline to add and THEN end
					on_newline = 1;
				}
				c = fgetc(x); // get next character
			}
		} // loop back to deal with next character
		l->text = malloc(sizeof(char) * btext_len);
		strncpy(l->text, btext, btext_len);
		free(btext); //! Check if this is safe
		btext_len = 0;
	}
	return buff; //? Is there preparation before return? Probably not, since it is naive.
}

/**
 * @brief Reads a file, allocates a buffer, and writes the file to it.
 * @note If the file does not exist, it is created.
 * @param filename A string that reflects a filename that leads to the file being read.
 * @return A buffer of type `struct buffer*`.
 */
struct buffer *bfread(char *filename);
// TODO: complete the definition of bfread.

int main() {
	struct buffer* x;
	x = bfread_n("tests/file2");
	printf("%sEND\n", x->head->text);
}
