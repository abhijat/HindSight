CC := gcc
CFLAGS := -D_XOPEN_SOURCE -D_BSD_SOURCE -g -Wall -Werror -pedantic -ansi 
#CFLAGS += -DDEBUG
SRC := triggers.c strlib.c

trigger: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o trigger

strlib: strlib.c
	$(CC) $(CFLAGS) strlib.c -o strlib

clean:
	rm -f trigger strlib *.o
