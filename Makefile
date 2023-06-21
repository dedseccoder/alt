CC=gcc
CFLAGS=-std=c11 -lcurl
TARGET = applicationAlt
LIBNAME = libhttpUtils.so

MAJOR = 1
MINOR = 0
PATCH = 1

all: $(TARGET)

install: $(TARGET)
	sudo mv -v bin/$(LIBNAME).$(MAJOR).$(MINOR).$(PATCH) /usr/local/lib/
	sudo mv -v bin/$(TARGET) /usr/local/bin/
	sudo ln -snf $(LIBNAME).$(MAJOR) /usr/local/lib/$(LIBNAME)
	sudo ldconfig /usr/local/lib/

$(TARGET): dirCreate $(LIBNAME) main.o
	ln -snf $(LIBNAME).$(MAJOR).$(MINOR).$(PATCH) ./bin/$(LIBNAME)
	$(CC) -lhttpUtils -L bin $(CFLAGS) bin/main.o -o bin/$@

$(LIBNAME): libhttpUtils.o
	$(CC)  bin/libhttpUtils.o $(CFLAGS) -fpic -shared -Wl,-soname,$@.$(MAJOR) -o bin/$@.$(MAJOR).$(MINOR).$(PATCH) -lc

main.o: cli/main.c
	$(CC) -c $(CFLAGS) $< -o bin/$@

libhttpUtils.o: lib/libhttpUtils.c
	$(CC) $(CFLAGS) -c -fpic $< -o bin/$@

dirCreate:
	@if [ ! -d bin ]; then\
		mkdir bin;\
	fi

clean:
	$(RM) -rfv bin/
