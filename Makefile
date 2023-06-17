CC=gcc
CFLAGS=-std=c11 -lcurl

all: applicationAlt

install: applicationAlt
	mv -v bin/*.so /usr/local/lib/
	mv -v bin/applicationAlt /usr/local/bin/
	sudo ldconfig /usr/local/lib

applicationAlt: libhttpUtils.so
	$(CC) -L./bin/ -o bin/$@ cli/main.c -lhttpUtils $(CFLAGS)

libhttpUtils.o: lib/libhttpUtils.c
	$(CC) $(CFLAGS) -c libhttpUtils.c

libhttpUtils.so: lib/libhttpUtils.c
	$(CC) $(CFLAGS) -fPIC -shared -o bin/$@ lib/libhttpUtils.c -lc

clean:
	$(RM) -rfv bin/*
	$(RM) -fv *.json