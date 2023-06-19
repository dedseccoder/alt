CC=gcc
CFLAGS=-std=c11 -lcurl
TARGET = applicationAlt

all: $(TARGET)

install: $(TARGET)
	sudo mv -v bin/*.so /usr/local/lib/
	sudo mv -v bin/$(TARGET) /usr/local/bin/
	sudo ldconfig /usr/local/lib

$(TARGET): dirCreate libhttpUtils.so main.o
	$(CC) -Lbin/ -lhttpUtils $(CFLAGS) bin/main.o -o bin/$@

libhttpUtils.so: libhttpUtils.o
	$(CC) $(CFLAGS) -fPIC -shared -lc bin/libhttpUtils.o -o bin/$@

main.o: cli/main.c
	$(CC) -c $(CFLAGS) $< -o bin/$@

libhttpUtils.o: lib/libhttpUtils.c
	$(CC) $(CFLAGS) -c -fPIC $< -o bin/$@

dirCreate:
	@if [ ! -d bin ]; then\
		mkdir bin;\
	fi

clean:
	$(RM) -rfv bin/
