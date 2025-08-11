#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

char curfile[256] = "file";
char error[512] = "";

struct line {
	char* text;
	int len;
	struct line* next; // pointer
	struct line* prev; // pointer
	// int mlen; // malloced
	int idx;
} gline;
void free_line(struct line* line);
void free_lines(struct line* line);
struct line* mallocl(FILE* fp);;
struct buffer {
	struct line** lines;
	int len;
	struct line* head; // pointer
	struct line* tail; // pointer
	// int mlen; // malloced
	char edited;
	int sub_buffer; // line # of parent, 0 = null
	char* filename;
	struct line* cur_address;
} buffer, cut;
void free_buffer(struct buffer* buff);
struct line** buffer_syncl2a(struct buffer* buff);
struct line** buffer_sync_list_to_array(struct buffer* buff) // API
{return buffer_syncl2a(buff);} // API
struct line* buffer_synca2l(struct buffer* buff);
struct line* buffer_sync_array_to_list(struct buffer* buff) // API
{return buffer_synca2l(buff);} // API
enum tokenstate {
	NONE,
	ADDRESS,
	ADDRESS_APP,
	REGEX,
	REGEX_ESCAPE,
	COMMAND,
	ARGUMENTS
};

const char* TOK_ADDRESS_BASE = "\\.|\\$|\\/.*\\/|\\?.*\\?|\'[a-z]";
const char* TOK_ADDRESS_APP = "( *(\\+|-|\\d*))* *";
const char* TOK_ADDRESS = "(?:\\.|\\$|\\/.*\\/|\\?.*\\?|\'[a-z]|\\d+) *(?:\\+ *|- *|\\d+ *)* *";
const char* TOK_ADDRESS_RANGE = "[,;]((?:\\.|\\$|\\/.*\\/|\\?.*\\?|\'[a-z]|\\d+) *(?:\\+ *|- *|\\d+ *)* *)?";

/* Extended Regular Expressions

\.|\$|\/.*\/|\?.*\?|'[a-z]
( *(\+|-|\d*))* *
(?:\.|\$|\/.*\/|\?.*\?|'[a-z]|\d+) *(?:\+ *|- *|\d+ *)* *
[,;]((?:\.|\$|\/.*\/|\?.*\?|'[a-z]|\d+) *(?:\+ *|- *|\d+ *)* *)?
((?:\.|\$|\/.*\/|\?.*\?|'[a-z]|\d+) *(?:\+ *|- *|\d+ *)* *)?[aikrxz=]?
([,;]((?:\.|\$|\/.*\/|\?.*\?|'[a-z]|\d+) *(?:\+ *|- *|\d+ *)* *)?)?[cdgGjlmnptvVwWy]|(?:wq)

*/

FILE* alread(char* filename);
struct buffer* bfread(char* filename, struct buffer* buff);
void bfwrite(char* filename, struct buffer* buff);
void ederror(char* reason);
int countlines(FILE* fp);


int main(){
	bfread(curfile, &buffer);
	buffer.lines[1]->text = "modification\n";
	buffer_syncl2a(&buffer);
	free_buffer(&buffer);
	return 0;
}

void ederror(char* reason){ // h will print \n after the error
	fprintf(stderr, "?\n");
	strcpy(error, reason);
}

FILE* alread(char* filename){ // always read
	FILE* fp;
	fp = fopen(filename, "r");
	if(fp == NULL){
		fp = fopen(filename, "w");
		fp = freopen(filename, "r", fp);
		// TODO: POSIX requires errno here?
	}
	return fp;
}

/// Allocates a line from a file into struct line. .next, .prev, and .idx must be set manually.
struct line* mallocl(FILE *fp){
	struct line* ret = malloc(sizeof(struct line));
	int x = 256; int len = 0; char *l = malloc(x); // memset(l, '\0', x);
	if (l == NULL){
		ederror("Cannot allocate current line");
		ret->text = ""; // -Wmaybe-uninitialized
		ret->len = 0;
		return ret;
	}
	int c = fgetc(fp);
	if(feof(fp)){
		fputc('\n', fp);
		l[len] = c;
		len++;
	}
	while (!feof(fp)){
		if (len + 1 >= x){
			x += 256;
			char *tmp = realloc(l, x);
			if (tmp == NULL){
				printf("mallocl %p", l);
				free(l);
				ederror("Cannot allocate enough memory");
				break;
			}
			l = tmp;
		}
		l[len] = c;
		len++;
		if (c == '\n'){
			break;
		}
		c = fgetc(fp);
	}
	l[len] = '\0';
	ret->text = l;
	ret->len = len;
	return ret;
}

int countlines(FILE* fp){
	fpos_t back;
	fgetpos(fp, &back);
	rewind(fp); fgetc(fp);
	if(feof(fp)){
		// rewind(fp);
		fputc('\n', fp);
		return 1;
	}
	rewind(fp);
	int lines = 0;
	while(!feof(fp)){
		lines += (fgetc(fp) == '\n'); // 10 = '\n'
	}
	fsetpos(fp, &back);
	return lines + 1;
}

struct buffer* bfread(char* filename, struct buffer* buff){
	// struct buffer buffer;
	FILE *fp = alread(filename);
	buff->lines = malloc(sizeof(struct line*) * countlines(fp));
	buff->len = countlines(fp);
	// synca2l buff->lines[0].prev = NULL;
	// synca2l buff->lines[buff->len - 1].next = NULL;
	for(int i = 0; i < buff->len; i++){
		buff->lines[i] = mallocl(fp);
	}
	fclose(fp);
	buff->edited = 0;
	buff->sub_buffer = 0;
	buff->filename = filename;
	buff->cur_address = buff->lines[0];
	buffer_synca2l(buff);
	return buff;
}

void bfwrite(char* filename, struct buffer* buff){
	FILE *fp = alread(filename);
	if(fp == NULL){
		ederror(strcat("Cannot open file ", filename));
	}
	for(int i = 0; i < buff->len; i++){
		fwrite(buff->lines[i]->text, 1, buff->lines[i]->len, fp);
	}
	fclose(fp);
}

// note: even though a buffer mallocs it's lines,
// every item in the array is malloced seperate from the array.
// thus wasting twice as much memory!
// todo: fix this, maybe making it struct line**? oof...
void free_lines(struct line* line){
	struct line *x, *y = (x = line->next);
	while(x != NULL){
		free_line(x);
		y = (x = y)->next;
	}
	x = line->prev;
	while(x != NULL){
		free_line(x);
		y = (x = y)->prev;
	}
	free_line(line);
}

// see free_lines
void free_line(struct line* line){
	printf("freeline %p %p\n", line->text, line);
	free(line->text);
	// Seperate function for deleting the line in a linked list
	free(line); // frees pointers and len
}

// see free_lines
void free_buffer(struct buffer* buff){
	int x = buff->len;
	for(int i = 0; i < x; i++){
		free_line(buff->lines[i]);
	}
	// free_lines(buff->head); // assume sync
	printf("freebuffer %p %p\n", buff->lines, buff);
	free(buff->lines);
	// free(&buff->len);
	free(buff);
}

// todo: fix these two functions (see free_lines)
/// Makes buffer.lines hold the values in the linked list
struct line* buffer_syncl2a(struct buffer* buff){
	struct line* x = buff->head;
	int y = 0;
	while(x != NULL){
		if(y >= buff->len){
			// In practice this won't happen
			buff->lines = realloc(buff->lines, sizeof(struct line) * (y + 1));
			// see free_lines
			buff->lines[y] = *(struct line*)malloc(sizeof(struct line));
		}
		buff->lines[y] = *x;
		x = x->next; y++;
	}
	buff->len = y;
	return buff->lines;
}

struct line* buffer_synca2l(struct buffer* buff){
	struct line* x = buff->head;
	if(x == NULL){
		x = buff->head = malloc(sizeof(struct line));
	}
	x->prev = NULL;
	for(int y = 0; y < buff->len; y++){
		if(x->next == NULL){
			x->next = malloc(sizeof(struct line));
		}
		x->next = &buff->lines[y];
		x->next->prev = x;
		x = x->next; // y++ in for-loop
		x->idx = y;
	}
	// y = buff->len, which is 1 over the last line
	printf("buffsync %p %p %p\n", x->prev, x, x->next);
	x = x->prev;
	x->next = NULL;
	buff->tail = x;
	return buff->head;
}

struct line* getlinel(struct buffer* buff, int idx){
	struct line* x = buff->head;
	while(x != NULL){
		if(x->idx == idx){
			return x;
		}
		x = x->next;
	}
	return NULL;
}