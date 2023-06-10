#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../lib/httpUtils.h"

int main(int argc, char **argv)
{
	if(!(argc == 3 || argc == 2)){
      exit(EXIT_FAILURE);
	}

	char url [2048] = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
	char* info;
	if(argv[2]){
		info = GET_Export(url, argv[1], argv[2]);
	}
	else{
		info = GET_Export(url, argv[1], NULL);
	}
	printf("%s\n", info);

	return 0;
}
