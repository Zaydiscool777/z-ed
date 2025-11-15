all: main
love: debug
	echo not war?

CC = gcc # can use tcc, gcc, or clang
# overide CFLAGS +=
CFILES = src/main.c
COUT = a.out

main: clean
	$(CC) $(CFILES) $(CFLAGS) -o $(COUT) -O3
	./$(COUT)

debug: clean
	$(CC) $(CFILES) $(CFLAGS) -o $(COUT) -g -O0 -v -Wall
	gdb $(COUT) --write

obj:
	objdump -afpPxDSsgeGtTrR $(COUT) > object

clean:
	rm -f $(COUT)
