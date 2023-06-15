#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>
#include "../lib/httpUtils.h"

typedef struct {
	 char* name;
	 time_t buildtime;
} value;

typedef struct {
    value v;
    struct node* next; 
} node;

typedef struct {
    node *tail; 
}list;




char* getValue(char*, char*, int*);
void InitList(list*);
void printList(list*);
void pushList(list*, char*, time_t);

int main(int argc, char **argv)
{
	if(!(argc == 3 || argc == 2)){
      exit(EXIT_FAILURE);
	}

	char* stringTags [2] = {"name", "buildtime"};

	char url [2048] = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
	char* info;
	if(argv[2]){
		info = GET_Export(url, argv[1], argv[2]);
	}
	else{
		info = GET_Export(url, argv[1], NULL);
	}

	list jsonInfo1;
    InitList(&jsonInfo1);
	int index = 0;

	while(*info != '\0'){
		char* data = getValue(info, stringTags[0], &index);
		info += index;
		char* buildtimestring = getValue(info, stringTags[1], &index);
		info += index;
		char *e;
		time_t buildtime = strtoll(buildtimestring, &e, 0);
		pushList(&jsonInfo1, data, buildtime);
	}
	printList(&jsonInfo1);
	printf("\n");

	free(index);
	free(info);
	return 0;
}

void InitList(list *sList)
{
    sList->tail = NULL;
}

void pushList(list *sList, char* name, time_t seconds)
{
    node *p;
    p = malloc(sizeof(node));
    p->v.name = name;
	p->v.buildtime = seconds;
    p->next = sList->tail;
    sList->tail = p;
}

void printList(list *sList)
{
    node *p = sList->tail;
    while(p != NULL) {
        printf("name: %s\n buildtime = %lld\n", p->v.name, p->v.buildtime);
        p = p->next;
    }
}

char* getValue(char* str, char* source, int *index)
{
	char* data = (char*)malloc(2048);
	char* out = data; 
	char* search = strstr(str, source);
	search = strstr(search, ":");
	*index = search - str;
	search++;
	while(*search == ' ') search++;
	if(*search == '"'){
		search++;
		while(*search  != '"' && *(search-1) != '\\'){
			*out = *search;
			out++;
			search++;
		}
	} 
	else{
		while(*search != ',' && *search != ' ' && *search != '\n' && *search != '}'){
			*out = *search;
			out++;
			search++;
		}
	}
	*out = '\0'; 

	if (data == out){
		free(data);
		return NULL;
	}

	return data;
}