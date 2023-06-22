#ifndef LIBHTTPJSON
#define LIBHTTPJSON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "libhttpUtils.h"

char *GET_Export(char *url, char *branch, char *_arch)
{
    if(!branch || !url){
        printf("branch name is empty\n");
        return "";
    }

    strcat(url, branch);

    if(_arch){
        strcat(url, "?arch=");
        strcat(url, _arch);
    }
    printf("%s\n",url);

    struct __url_data url_data;
    url_data.data = NULL;
    url_data.size = 0;

    CURL *curl = curl_easy_init();
    if(!curl){
        printf("curl fail\n");
        curl_easy_cleanup(curl);
        return "";
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPGET,1); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __got_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &url_data);

    CURLcode responseCode = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
            
    if(responseCode != CURLE_OK){
        printf ("responseCode fail: %d\n", responseCode);
        printf ("http_code fail: %d\n", http_code);
        curl_easy_cleanup(curl);
        return "";
    }
    printf("http_code: %ld\n", http_code);
    curl_easy_cleanup(curl);
    return url_data.data;
}

size_t __got_data(char *buffer, size_t itemSize, size_t nitems, void *userp)
{
    size_t bytes = itemSize * nitems;
    struct __url_data *mem = (struct __url_data*) userp; 
    char *ptr = realloc(mem->data, mem->size + bytes + 1);
    if(!ptr){
        return 0;
    }
    mem->data = ptr;
    memcpy(&mem->data[mem->size], buffer, bytes);
    mem->size += bytes;
    mem->data[mem->size] = 0;

    return bytes;
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

#endif