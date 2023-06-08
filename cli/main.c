#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "../lib/httpUtils.h"

int main(int argc, char **argv)
{
	if(argc != 3){
      exit(EXIT_FAILURE);
	}

	char* url = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
	char* info = GET_Export(url, argv[1], argv[2]);
	printf("%s\n", info);
	
	return 0;
}
