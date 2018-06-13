
CFLAGS  = -g --std=c99 -Wall -I.. -D_XOPEN_SOURCE
LDFLAGS  = -L.. -lmpegts

all:	libtest

libtest:	libtest.c
	gcc $(CFLAGS) $(@).c -o $(@) $(LDFLAGS)

clean:
	rm -f libtest

test:	libtest
	valgrind --leak-check=full ./libtest

