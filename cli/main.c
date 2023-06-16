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
int countDubStr(char*, char*);
void getOnly(list*, list*, list*);
void getFreshest(list*, list*, list*);
void writeJsonFile(list*, list*, list*);

int main(int argc, char **argv)
{
	if(!(argc == 4 || argc == 3)){
      exit(EXIT_FAILURE);
	}

	char* stringTags [2] = {"name", "buildtime"};

	char url [2048] = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
	char* info;
	if(argv[3]){
		info = GET_Export(url, argv[1], argv[3]);
	}
	else{
		info = GET_Export(url, argv[1], NULL);
	}

	list jsonInfoList1, jsonInfoList2;

    InitList(&jsonInfoList1);
	int index = 0;
	int names = countDubStr(info, "{\"name\":");
	int i = 0;
	while(i < names){
		char* data = getValue(info, stringTags[0], &index);
		info += index;
		char* buildtimestring = getValue(info, stringTags[1], &index);
		info += index;
		char *e;
		time_t buildtime = strtoll(buildtimestring, &e, 0);
		pushList(&jsonInfoList1, data, buildtime);
		i++;
	}

	index = 0;
	names = 0;
	i = 0;

	char* info2;
	if(argv[3]){
		info2 = GET_Export(url, argv[2], argv[3]);
	}
	else{
		info2 = GET_Export(url, argv[2], NULL);
	}

	InitList(&jsonInfoList2);
	while(i < names){
		char* data = getValue(info2, stringTags[0], &index);
		info2 += index;
		char* buildtimestring = getValue(info2, stringTags[1], &index);
		info2 += index;
		char *e;
		time_t buildtime = strtoll(buildtimestring, &e, 0);
		pushList(&jsonInfoList2, data, buildtime);
		i++;
	}

	list onlyA, onlyB, freshList;
	InitList(&onlyA);
	InitList(&onlyB);
	InitList(&freshList);

	getOnly(&jsonInfoList1, &jsonInfoList2, &onlyA);
	getOnly(&jsonInfoList1, &jsonInfoList2, &onlyB);
	getFreshest(&jsonInfoList1, &jsonInfoList2, &freshList);
	writeJsonFile(&onlyA, &onlyB, &freshList);

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
        printf("name: %s\n buildtime = %lld\n\n", p->v.name, p->v.buildtime);
        p = p->next;
    }
}

void getOnly(list* jsonInfo1, list* jsonInfo2, list* only)
{
	node *p1 = jsonInfo1->tail;
	node *p2 = jsonInfo2->tail;

	int dub = 1;
	while(p1 != NULL){
		while(p2 != NULL){
			if(p1->v.name == p2->v.name){
				dub = 0;
			}
			p2 = p2->next;
		}
		p2 = jsonInfo2->tail;
		if(dub){
			pushList(only, p1->v.name, p1->v.buildtime);
		}
		p1 = p1->next;
	}	
}

void getFreshest(list* jsonInfo1, list* jsonInfo2, list* freshList)
{
	node *p1 = jsonInfo1->tail;
	node *p2 = jsonInfo2->tail;

	int unique = 1;
	while(p1 != NULL){
		while(p2 != NULL){
			if(p1->v.name == p2->v.name){
				unique = 0;
				if(p1->v.buildtime >= p2->v.buildtime){
					pushList(freshList, p1->v.name, p1->v.buildtime);
				}
				else{
					pushList(freshList, p2->v.name, p2->v.buildtime);
				}
				break;
			}
			p2 = p2->next;
		}
		p2 = jsonInfo2->tail;
		if(unique){
			pushList(freshList, p1->v.name, p1->v.buildtime);
		}
		p1 = p1->next;
	}
	p1 = jsonInfo1->tail;
	p2 = jsonInfo2->tail;
	unique = 1;
	while (p2 != NULL){
		while (p1 != NULL){
			if(p1->v.name == p2->v.name){
				unique = 0;
				break;
			}
			p1 = p1->next;
		}
		p1 = jsonInfo1->tail;
		if(unique){
			pushList(freshList, p2->v.name, p2->v.buildtime);
		}
		p2 = p2->next;
	}
	
}

int countDubStr(char* str, char* search)
{
	int found;
	int count = 0;
	int len1 = strlen(str);
	int len2 = strlen(search);
	for(int i = 0; i <= len1 - len2; i++){
		found = 1;
		for(int j = 0; j < len2; j++){
			if(str[i+j] != search[j]){
				found = 0;
				break;
			}
		}
		if(found == 1){
			count++;
		}
	}
	return count;
};

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