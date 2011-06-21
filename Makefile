CC := gcc
CFLAGS := -D_XOPEN_SOURCE -D_BSD_SOURCE -g -Wall -Werror -pedantic -ansi 
#CFLAGS += -DDEBUG
SRC := triggers.c strlib.c

trigger: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o trigger

strlib: strlib.c strlib.h
	$(CC) $(CFLAGS) strlib.c -o strlib

filelib: filelib.c filelib.h strlib.c strlib.h
	$(CC) $(CFLAGS) filelib.c strlib.c -o filelib

clean:
	rm -f trigger strlib filelib *.o
