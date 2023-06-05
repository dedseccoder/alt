CC=g++
CFLAGS=-lcurl

all:start

start: liber
	$(CC) -o bin/application cli/main.cpp $(CFLAGS)

liber:
	$(CC) -c lib/httpUtils.cpp -o bin/http.o $(CFLAGS)
	$(CC) -c lib/httpUtils.h -o bin/httpH.o $(CFLAGS)

clean:
	$(RM) -rf bin/*