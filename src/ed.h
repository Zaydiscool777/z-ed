#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h> // note: use dlerror and put in ed error system
#include <string.h>
#include <ctype.h>
#include <signal.h> // especially Ctrl-D and Ctrl-C. use sigaction, not signal
#include <regex.h>

// # types

/**
 * @brief Type synonym for an address in the buffer.
 * @typedef int
 */
typedef int addr;

/**
 * @struct line
 * @brief Context and data of a textual line as an element in a doubly linked list.
 * @param text Text in the line, or NULL if there is none.
 * @param next Pointer to the next line, or NULL if there is none.
 * @param prev Pointer to the previous line, or NULL if there is none.
 * @deprecated `idx` used to be the index of the line in the buffer, or 0 if lacking a proper one.
 * @deprecated `buffer` used to be the buffer the line resides in.
 * @warning The text must be null-terminated, like the ones in C. This will be fixed in the future.
 */
struct line {
	char *text; /// Text in the line, or NULL if there is none.
	struct line *next; /// Pointer to the next line, or NULL if there is none.
	struct line *prev; /// Pointer to the previous line, or NULL if there is none.
};

/**
 * @struct buffer
 * @brief Context and data of a textual buffer that holds all of its lines.
 * @param head Pointer to the first line in the buffer.
 * @param tail Pointer to the last line of the buffer.
 * @deprecated `array` used to be a list of lines as an array.
 * @remark Perhaps `size` can also be added.
 * @note If the buffer is empty, both `head` and `tail` should be NULL. Usually, `head` is checked for this.
 */
struct buffer {
	struct line *head; /// Pointer to the first line of the buffer.
	struct line *tail; /// Pointer to the last line of the buffer.
};

/**
 * @struct addrr
 * @brief A range of addresses, or a single address, in a buffer, usually passed to commands to act upon.
 * @param start The start of the range, or the address if a single one.
 * @param end The end of the range, or the address if a single one.
 * @note For a single address, the `start` and `end` are both the same, and are both the address.
 * The is also true the other way around. Check the `end` for the single address.
 * @example `1` becomes `{1, 1}`, where `start` and `end` are both 1.
 * @example `4;++` becomes `{4, 6}`, where `start` is 4, and `end` is 6.
 */
struct addrr {
	addr start; /// The start of the range, or the address if a single one.
	addr end; /// The end of the range, or the address if a single one.
};

/**
 * @struct command
 * @brief Command data that will be used to locate, execute, and provide inputs to commands.
 * @param name The letter that identifies the command, or NUL in the case that none is given.
 * @param range The range that the command acts upon, usually entered before the command name.
 * @param args The extra arguments that the command may use, usually entered after the command name.
 * @attention It is better to assign args to an empty string instead of NULL.
 * @example `1,3m2p` becomes `{'m', {1, 3}, "2p"}`.
 */
struct command {
	char name; /// The letter that identifies the command, or NUL in the case that none is given.
	struct addrr range; /// The range that the command acts upon, usually entered before the command name.
	char *args; /// The extra arguments that the command may use, usually entered after the command name.
};

/**
 * @struct parse
 * @brief Result of parsing a command input line.
 * @param ok `PARSE_OK` if parsing succeeds, or some different value if it fails.
 * @param cont Pointer to the remainder of the input after the parsed command data, if it succeeds.
 */
struct parse {
	int ok; /// `PARSE_OK` if parsing succeeds, or some different value if it fails.
	char *cont; /// Pointer to the remainder of the input after the parsed command data, if it succeeds.
};

/**
 * @brief Parse success of the `parse` struct.
 * @enum PARSE
 * @param PARSE_OK Success.
 * @param PARSE_FAIL_GENERAL General failure.
 * @param PARSE_UNEXPECTED_NUL A NUL character was found when something else was expected.
 * @param PARSE_INVALID_MARK A lowercase letter did not follow an apostrophe used to denote a mark address.
 * @param PARSE_UNEXPECTED_NEWLINE A newline character was found when something else was expected.
 * @param PARSE_MAXVALUE Maximum value of the PARSE enumeration.
 */
enum PARSE {
	PARSE_OK, /// Success.
	PARSE_FAIL_GENERAL, /// General failure.
	PARSE_UNEXPECTED_NUL, /// A NUL character was found when something else was expected.
	PARSE_INVALID_MARK, /// A lowercase letter did not follow an apostrophe used to denote a mark address.
	PARSE_UNEXPECTED_NEWLINE, /// A newline character was found when something else was expected.
	PARSE_MAXVALUE /// Maximum value of the PARSE enumeration.
};

// # lib.c

void lib_init(void);

void lib_term(void);

// command processing

struct parse find_comm(char *inp);

void load(struct command comm);

extern void comm_exit(void);

// error handling

extern char *ed_error;

void set_ed_error(char *s);

// line and buffer operations

struct line *buffer_index(struct buffer buff, addr i);

void buffer_insert_after(struct line *after, struct buffer *in, struct buffer *new);

void buffer_insert_before(struct line *before, struct buffer *in, struct buffer *new);

void buffer_remove(struct buffer *in, struct buffer *old);

void buffer_delete(struct buffer *in, struct buffer *bad);

struct buffer buffer_read_file(char *fname);

struct buffer buffer_read_input(void);

struct line *buffer_search_forward(struct buffer in, struct line *at, regex_t *match);

struct line *buffer_search_backward(struct buffer in, struct line *at, regex_t *match);

// # standard ed

// variables

int help_mode;

int ran_global_comm;

int mod_buffer;

struct buffer current_buffer;

struct buffer undo;

struct buffer cut;

char *filename;

char *last_regex_f;

addr marks[26];

char *last_shell_esc;

int window_size;

addr current_addr;
