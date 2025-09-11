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
struct line
{
	char *text;
	struct line *next;
	struct line *prev;
	// unsigned int idx;
};


/// @brief Context and data of a textual buffer that holds all of its lines.
struct buffer
{
	/// @param array List of lines as an array. To use in the future.
	// struct line *array;
	/// @param head Pointer to the first line in the buffer.
	struct line *head;
};

struct buffer *bfread(char *filename)
{
	FILE *x = fopen(filename, "r");
	if(x == NULL)
	{
		fclose(x);
		x = fopen(filename, "x+");
		fclose(x);
		x = fopen(filename, "r");
	}
	struct buffer *buff = malloc(sizeof(struct buffer));
	buff->head = NULL;
	while(!feof(x))
	{
		struct line* l = malloc(sizeof(struct line));
		l->text = NULL;
		l->next = NULL;
		l->prev = NULL;
		char *btext = NULL;
		while(fgets(btext))

	}
}

int main()
{
	return 0;
}