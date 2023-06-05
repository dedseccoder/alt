#include<iostream>
#include<string>
#include"../lib/httpUtils.cpp"
#include <curl/curl.h>

using std::cout;  
using std::endl;

int main(int argc, char **argv)
{
	if(argc != 3){
      exit(EXIT_FAILURE);
	}

	httpUtils_H *api = new httpUtilsAlt();
	api->GET(argv[1], argv[2]);
	
	delete api;
	return 0;
}
