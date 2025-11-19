
#define NULL ((void *)0)

/**
 * @brief Type synonym for an address in the buffer.
 * @typedef unsigned int
 */
typedef unsigned int addr;

/**
 * @struct line
 * @brief Context and data of a textual line as an element in a doubly linked list.
 * @param text Text in the line, or NULL if there is none.
 * @param next Pointer to the next line, or NULL if there is none.
 * @param prev Pointer to the previous line, or NULL if there is none.
 * @deprecated idx used to be the index of the line in the buffer, or 0 if lacking a proper one.
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
 * @deprecated `array` used to be a list of lines as an array.
 * @remark Perhaps `tail` and `size` can also be added.
 */
struct buffer {
	struct line *head; /// Pointer to the first line in the buffer.
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
 * @brief Comand data that will be used to locate, execute, and provide inputs to commands.
 * @param name The letter that identifies the command, or NUL in the rare case that none is given.
 * @param range The range that the command acts upon, usually entered before the command name.
 * @param args The extra arguments that the command may use, usually entered after the command name.
 * @attention It is better to assign args to an empty string instead of NULL.
 * @example `1,3m2p` becomes `{'m', {1, 3}, "2p"}`.
 */
struct command {
	char name; /// The letter that identifies the command, or NUL in the rare case that none is given.
	//* some commands are more than one letter long, e.g. `wq`
	struct addrr range; /// The range that the command acts upon, usually entered before the command name.
	char *args; /// The extra arguments that the command may use, usually entered after the command name.
};

struct parse_addr {
	addr res;
	char *cont;
};
