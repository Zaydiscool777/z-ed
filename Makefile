all: main
love: debug

CC = gcc # can use tcc, gcc, or clang
# overide CFLAGS +=
CFILES = main.c

main: clean
	$(CC) $(CFILES) $(CFLAGS)
	./a.out

debug: clean
	$(CC) $(CFILES) $(CFLAGS) -g -Og -v
	gdb ./a.out

obj:
	objdump -afpPxDSsgeGtTrR ./a.out > object

clean:
	rm -f a.out