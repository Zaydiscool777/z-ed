all: main

CC = clang
# overide CFLAGS +=
CFILES = main.c

main:
	$(CC) $(CFLAGS) -o ./main $(CFILES)
	./main

debug:
	$(CC) $(CFLAGS) -O0 -v -o ./main-debug $(CFILES)
	./main-debug
	
clean:
	rm -f main main-debug