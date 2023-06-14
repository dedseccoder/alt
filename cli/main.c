#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>
#include "../lib/httpUtils.h"

char* getValue(char*, char*);

typedef struct {
	 char* name;
	 time_t buildtime;
} value;

typedef struct node {
    value v;
    struct node* next; 
};

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

	for(int i = 0; i < 2; i++)
	{
		printf("%s: %s\n", stringTags[i], getValue(info, stringTags[i]));
	}

	free(info);
	return 0;
}


char* getValue(char* str, char* source)
{
	char* data = (char*)malloc(2048);
	char* out = data; 
	char* search = strstr(str, source);
	search = strstr(search, ":");
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