all: main
love: debug
	echo not war?

CC = gcc # can use tcc, gcc, or clang
# overide CFLAGS +=
CFILES = main.c

main: clean
	$(CC) $(CFILES) $(CFLAGS) -O3
	./a.out

debug: clean
	$(CC) $(CFILES) $(CFLAGS) -g -O0 -v -Wall
	gdb ./a.out --write

obj:
	objdump -afpPxDSsgeGtTrR ./a.out > object

clean:
	rm -f a.out