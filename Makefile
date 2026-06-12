all: main
love: debug
	echo not war?

CC = gcc # can use tcc, gcc, or clang
# overide CFLAGS +=
CFILES = src/commproc.c
COUT = a.out

main: clean
	$(CC) $(CFLAGS) $(CFILES) -o $(COUT)
	chmod +x $(COUT)
	./$(COUT)

debug: clean
	$(CC) $(CFLAGS) $(CFILES) -o $(COUT) -g -O0 -v -Wall
	chmod 777 $(COUT)
	# gdb $(COUT) --write

obj:
	objdump -afpxDSsgeGtTrR $(COUT) > object

.PHONY: main debug obj all love clean

clean:
	rm -f $(COUT)
