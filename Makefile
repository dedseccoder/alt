CC=gcc
CFLAGS=-std=c11 -lcurl

all:start

start: liber
	$(CC) -o bin/application cli/main.c $(CFLAGS)

liber:
	$(CC) -c lib/httpUtils.h -o bin/httpH.o $(CFLAGS)

clean:
	$(RM) -rf bin/*