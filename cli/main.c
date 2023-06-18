#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>
#include "../lib/libhttpUtils.c"

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



char* getValue(char*, char*, int*);
void InitList(list*);
void printList(list*);
void pushList(list*, char*, time_t, char*);
int countDubStr(char*, char*);
void getOnly(list*, list*, list*);
void getFreshest(list*, list*, list*);
void writeJsonFile(list*, list*, list*, char*, char*, char*, char*);

int main(int argc, char **argv)
{
	if(!(argc == 5 || argc == 4 || argc == 3)){
      exit(EXIT_FAILURE);
	}

	char* stringTags [3] = {"name", "version", "buildtime"};

	char url [2048] = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
	char* info;
	if(argv[3]){
		info = GET_Export(url, argv[1], argv[3]);
	}
	else{
		info = GET_Export(url, argv[1], NULL);
	}
	url[0] = '\0';
	strcat(url, "https://rdb.altlinux.org/api/export/branch_binary_packages/");
	list jsonInfoList1, jsonInfoList2;

    InitList(&jsonInfoList1);
	int index = 0;
	int names = countDubStr(info, "{\"name\":");
	int i = 0;
	while(i < names){
		char* data = getValue(info, stringTags[0], &index);
		info += index;
		char* version = getValue(info, stringTags[1], &index);
		info += index;
		char* buildtimestring  = getValue(info, stringTags[2], &index);
		info += index;
		
		char *e;
		time_t buildtime = strtoll(buildtimestring, &e, 0);
		pushList(&jsonInfoList1, data, buildtime, version);
		i++;
	}

	index = 0;
	names = 0;
	i = 0;

	char* info2;
	if(argv[3]){
		if(argv[4]){
			info2 = GET_Export(url, argv[2], argv[4]);
		}
		else{
			info2 = GET_Export(url, argv[2], argv[3]);
		}
	}
	else{
		info2 = GET_Export(url, argv[2], NULL);
	}

	names = countDubStr(info2, "{\"name\":");
	InitList(&jsonInfoList2);
	while(i < names){
		char* data = getValue(info2, stringTags[0], &index);
		info2 += index;
		char* version = getValue(info2, stringTags[1], &index);
		info2 += index;
		char* buildtimestring  = getValue(info2, stringTags[2], &index);
		info2 += index;

		char *e;
		time_t buildtime = strtoll(buildtimestring, &e, 0);
		pushList(&jsonInfoList2, data, buildtime, version);
		i++;
	}

	list onlyA, onlyB, freshList;
	InitList(&onlyA);
	InitList(&onlyB);
	InitList(&freshList);

	printf("Sorting...\n");
	getOnly(&jsonInfoList1, &jsonInfoList2, &onlyA);
	printf("created list A\n");
	getOnly(&jsonInfoList2, &jsonInfoList1, &onlyB);
	printf("created list B\n");
	getFreshest(&jsonInfoList1, &jsonInfoList2, &freshList);
	printf("created freshest list\n");
	printf("Writing JSON file...\n");
	if(argv[3]){
		if(argv[4]){
			writeJsonFile(&onlyA, &onlyB, &freshList, argv[1], argv[2], argv[3], argv[4]);
		}
		else{
			writeJsonFile(&onlyA, &onlyB, &freshList, argv[1], argv[2], argv[3], NULL);
		}
	}
	else{
		writeJsonFile(&onlyA, &onlyB, &freshList, argv[1], argv[2], NULL, NULL);
	}

	return 0;
}

void InitList(list *sList)
{
    sList->tail = NULL;
}

void pushList(list *sList, char* name, time_t seconds, char* version)
{
    node *p;
    p = malloc(sizeof(node));

    p->v.name = name;
	p->v.buildtime = seconds;
	p->v.version = version;

    p->next = sList->tail;
    sList->tail = p;
}

void printList(list *sList)
{
    node *p = sList->tail;
    while(p != NULL) {
        printf("name: %s\nbuildtime = %lld\nversion = %s\n\n", p->v.name, p->v.buildtime, p->v.version);
        p = p->next;
    }
}

void writeJsonFile(list* onlyA, list* onlyB, list* freshList, char* branchA, char* branchB, char* _arch, char* _arch2)
{
	node *p1 = onlyA->tail;
	node *p2 = onlyB->tail;
	node *p3 = freshList->tail;
	char filename[1024] = "alt-export-";

	strcat(filename, branchA);
	strcat(filename, "-");
	strcat(filename, branchB);

	if(_arch){
		strcat(filename, "-");
		strcat(filename, _arch);
	}
	if(_arch2){
		strcat(filename, "-");
		strcat(filename, _arch2);
	}

	strcat(filename, ".json");
	FILE *pf = fopen(filename, "w");
	fprintf(pf, "{\n");

	fprintf(pf, "\t\"%s\": [", branchA);
	while(p1 != NULL){
		fprintf(pf, "{\"name\": \"%s\", \"version\": \"%s\", \"buildtime\": %lld}", p1->v.name, p1->v.version, p1->v.buildtime);
		p1 = p1->next;
		if(p1 != NULL){
			fprintf(pf, ", ");
		}
	}
	fprintf(pf, "],\n");
	printf("Branch %s is done\n", branchA);

	fprintf(pf, "\t\"%s\": [", branchB);
	while(p2 != NULL){
		fprintf(pf, "{\"name\": \"%s\", \"version\": \"%s\", \"buildtime\": %lld}", p2->v.name, p2->v.version, p2->v.buildtime);
		p2 = p2->next;
		if(p2 != NULL){
			fprintf(pf, ", ");
		}
	}
	fprintf(pf, "],\n");
	printf("Branch %s is done\n", branchB);

	fprintf(pf, "\t\"FreshList\": [");
	while(p3 != NULL){
		fprintf(pf, "{\"name\": \"%s\", \"version\": \"%s\", \"buildtime\": %lld}", p3->v.name, p3->v.version, p3->v.buildtime);
		p3 = p3->next;
		if(p3 != NULL){
			fprintf(pf, ", ");
		}
	}
	fprintf(pf, "]");
	printf("Freshest list is done\n");

	fprintf(pf, "\n}");
	fclose(pf);
	printf("JSON file complete\n");
}

void getOnly(list* jsonInfo1, list* jsonInfo2, list* only)
{
	node *p1 = jsonInfo1->tail;
	node *p2 = jsonInfo2->tail;

	int unique;
	while(p1 != NULL){
		unique = 1;
		while(p2 != NULL){
			if(strcmp(p1->v.name, p2->v.name) == 0){
				unique = 0;
			}
			p2 = p2->next;
		}
		p2 = jsonInfo2->tail;
		if(unique != 0){
			pushList(only, p1->v.name, p1->v.buildtime, p1->v.version);
		}
		p1 = p1->next;
	}
}

void getFreshest(list* jsonInfo1, list* jsonInfo2, list* freshList)
{
	node *p1 = jsonInfo1->tail;
	node *p2 = jsonInfo2->tail;

	int unique;
	while(p1 != NULL){
		unique = 1;
		while(p2 != NULL){
			if(strcmp(p1->v.name, p2->v.name) == 0){
				unique = 0;
				if(p1->v.buildtime > p2->v.buildtime){
					pushList(freshList, p1->v.name, p1->v.buildtime, p1->v.version);
				}
				else{
					pushList(freshList, p2->v.name, p2->v.buildtime, p2->v.version);
				}
				break;
			}
			p2 = p2->next;
		}
		p2 = jsonInfo2->tail;
		if(unique){
			pushList(freshList, p1->v.name, p1->v.buildtime, p1->v.version);
		}
		p1 = p1->next;
	}
	p1 = jsonInfo1->tail;
	p2 = jsonInfo2->tail;
	while (p2 != NULL){
		unique = 1;
		while (p1 != NULL){
			if(strcmp(p1->v.name, p2->v.name) == 0){
				unique = 0;
				break;
			}
			p1 = p1->next;
		}
		p1 = jsonInfo1->tail;
		if(unique){
			pushList(freshList, p2->v.name, p2->v.buildtime, p2->v.version);
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