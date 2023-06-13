#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../lib/httpUtils.h"

char* searchString(char*, char*);

int main(int argc, char **argv)
{
	if(!(argc == 3 || argc == 2)){
      exit(EXIT_FAILURE);
	}

	char* stringTags [8] = { "name", "epoch", "version", "release", "arch", "disttag", "buildtime", "source"};

	char url [2048] = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
	char* info;
	if(argv[2]){
		info = GET_Export(url, argv[1], argv[2]);
	}
	else{
		info = GET_Export(url, argv[1], NULL);
	}

	for(int i = 0; i < 8; i++){
		printf("%zu\n",strlen(info));
		printf("%s: %s\n", stringTags[i], searchString(info, stringTags[i]));
	}

	free(info);
	return 0;
}


char* searchString(char* str, char* source)
{
	char* res = strstr(str, source);
	char* swap = strdup(str);
	
  	char* pch  = strtok(swap, " :,\"[]{}");
	printf("method %zu\n",strlen(str));
	pch = strtok(NULL, " :,\"{}[]");
	return pch;
}