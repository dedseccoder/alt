#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>
#include "../lib/libhttpUtils.h"

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