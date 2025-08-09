#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char curfile[256] = "file";
char error[512] = "";

struct line {
	char* text;
	long unsigned int len;
	struct line* next; // pointer
	struct line* prev; // pointer
} gline;
void free_lines(struct line* line);
void free_linen(struct line* line);
void free_line(struct line* line);
struct line mallocl(FILE* fp);;
struct buffer {
	struct line* lines; // array
	int len;
	struct line* head; // pointer
	struct line* tail; // pointer
} buffer, cut;
void free_buffer(struct buffer* buff);
struct line* buffer_syncl2a(struct buffer* buff);
struct line* buffer_sync_list_to_array(struct buffer* buff){return buffer_syncl2a(buff);} /* API */
struct line* buffer_synca2l(struct buffer* buff);
struct line* buffer_sync_array_to_list(struct buffer* buff){return buffer_synca2l(buff);}

void ederror(char* reason);
int countlines(FILE* fp);
FILE* alread(char* filename);
struct buffer bfread(char* filename);
void bfwrite(char* filename);

int main(){
	struct buffer h = bfread(curfile);
	
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

struct line mallocl(FILE *fp){
	struct line ret;
	int x = 256; int len = 0; char *l = malloc(x); // memset(l, '\0', x);
	if (l == NULL){
		ederror("?\n");
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
				free(l);
				ederror("Cannot allocate enough memory");
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
	ret.text = l;
	ret.len = len;
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

struct buffer bfread(char* filename){
	// struct buffer buffer;
	FILE *fp = alread(filename);
	buffer.lines = malloc(sizeof(struct line) * countlines(fp));
	buffer.len = countlines(fp);
	for(int i = 0; i < buffer.len; i++){
		// using countlines(fp) calls it every time, rewinding the file
		buffer.lines[i] = mallocl(fp);
	}
	fclose(fp);
	return buffer;
}

void bfwrite(char* filename){
	FILE *fp = alread(filename);
	if(fp == NULL){
		ederror(strcat("Cannot open file ", filename));
	}
	for(int i = 0; i < buffer.len; i++){
		fwrite(buffer.lines[i].text, 1, buffer.lines[i].len, fp);
	}
	fclose(fp);
}

// See free_linen for why this is longer than you think
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

// A naïve version of free_line ("The Haskell way")
void free_linen(struct line* line){
	free(line->text);
	free_line(line->next);
	free_line(line->prev);
	free(line);
}

void free_line(struct line* line){
	free(line->text);
	// Seperate function for deleting the line in a linked list
	free(line); // frees pointers and len
}

void free_buffer(struct buffer* buff){
	int x = buff->len;
	for(int i = 0; i < x; i++){
		free_line(&buff->lines[i]);
	}
	free(buff->lines);
	struct line* y = buff->head;
	// free(&buff->len);
	free(buff);
}

/// Makes .lines hold the values in the linked list
struct line* buffer_syncl2a(struct buffer* buff){
	struct line* x = buff->head;
	int y = 0;
	while(x != NULL){
		if(y >= buff->len){
			// In practice this won't happen
			buff->lines = realloc(buff->lines, sizeof(struct line) * (y + 1));
		}
		buff->lines[y] = *x;
		x = x->next; y++;
	}
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
	}
	// y = buff->len, which is 1 over the last line
	x = x->prev; free(x->next); x->next = NULL;
	buff->tail = x;
	return buff->head;
}