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

#endif