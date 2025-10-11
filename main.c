#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFTEXTSIZE 1024

/**
 * @struct line
 * @brief Context and data of a textual line as an element is a double linked list.
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


/// @brief Context and data of a textual buffer that holds all of its lines.
/// @param head Pointer to the first line in the buffer.
struct buffer {
	/// @param head Pointer to the first line in the buffer.
	struct line *head;
	/// @param array List of lines as an array. To use in the future.
	// struct line *array;
};

/**
 * @brief Reads a file, allocates a buffer, and writes the file to it naively.
 * @warning This only updates the text and next pointers, but not the previous pointers.
 * @note If the file does not exist, it is created.
 * @param filename 
 * @return struct buffer* 
 */
struct buffer *bfread_n(char *filename) {
	FILE *x = fopen(filename, "r"); // opens file conditionally
	if(x == NULL) { // if file doesn't exist, create it and then open it
		fclose(x);
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
		char *btext = malloc(sizeof(char) * 1);
		size_t btext_len = sizeof(char) * 1;
		btext[0] = '\0';
		char c = '\0';
		c = fgetc(x);
		int on_newline = 0;
		while(!on_newline && c != '\0' && !feof(x)) {
			btext_len += sizeof(char);
			void *realloc_check;
			realloc_check = realloc(btext, btext_len);
			if(realloc_check == NULL) {
				printf("error: reallocation failed. btext_len: %lu\n", btext_len);
			} else {
				btext[btext_len - 2] = c;
				btext[btext_len - 1] = '\0';
				if(c == '\n') {
					on_newline = 1;
				}
				c = fgetc(x);
			}
		}
		l->text = malloc(sizeof(char) * btext_len);
		strncpy(l->text, btext, btext_len);
	}
	return buff; //?
}

int main()
{
	struct buffer* x;
	x = bfread_n("file2");
	printf("%sEND\n", x->head->text);
}