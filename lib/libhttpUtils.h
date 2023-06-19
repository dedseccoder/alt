#ifndef LIBHTTP_PROTO
#define LIBHTTP_PROTO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	 char* name;
	 char* version;
	 time_t buildtime;
}value;

typedef struct {
    value v;
    struct node* next; 
}node;

typedef struct {
    node *tail; 
}list;

struct __url_data {
    size_t size;
    char* data;
};

char* getValue(char*, char*, int*);
void InitList(list*);
void printList(list*);
void pushList(list*, char*, time_t, char*);
int countDubStr(char*, char*);
void getOnly(list*, list*, list*);
void getFreshest(list*, list*, list*);
void writeJsonFile(list*, list*, list*, char*, char*, char*, char*);

char *GET_Export(char *, char *, char *);
size_t __got_data(char*, size_t, size_t, void*);

#endif