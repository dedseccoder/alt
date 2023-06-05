#include <iostream>
#include <string>
#include <curl/curl.h>
#include "httpUtils.h"

using std::cout;  
using std::endl;
using std::string;

//https://rdb.altlinux.org/api/export/branch_binary_packages/p10?arch=x86_64

class httpUtilsAlt : public httpUtils_H
{

    public:
        void GET(string branch, string arch="") override
        {
            if(branch.empty()){
                cout << "branch name is empty" << endl;
                return;
            } 
            header.append(branch);
            if(!arch.empty()){
                header.append("?arch=" + arch);
            }
            cout << header << endl;
            CURL *curl = curl_easy_init();
            if(!curl){
                cout << "curl fail" << endl;
                curl_easy_cleanup(curl);
                return;
            }
            curl_easy_setopt(curl, CURLOPT_URL, header.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);

            CURLcode responseCode = curl_easy_perform(curl);
            long http_code = 0;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
            
            if(responseCode != CURLE_OK){
                cout << "responseCode fail: " << responseCode << endl;
                cout << "http_code fail: " << http_code << endl;
                curl_easy_cleanup(curl);
                return;
            }
            cout << "http_code: " << http_code << endl;

            curl_easy_cleanup(curl);
            return;
        }

    private:
        
        string header = "https://rdb.altlinux.org/api/export/branch_binary_packages/";

        static size_t got_data(char *buffer, size_t itemSize, size_t nitems, void *ignoreThis)
        {
            size_t bytes = itemSize * nitems;
            
            int linenumber = 1;
            for(int i = 0; i < bytes; i++){
                cout << buffer[i];
                if(buffer[i] == '\n'){
                    linenumber++;
                    cout << "line:" << linenumber;
                }
            }
            cout << "\n\n";
            return bytes;            
        }
};