#include <iostream>
#include <string>
#include <curl/curl.h>
#include "httpUtils.h"

using std::cout;  
using std::endl;
using std::string;

class httpUtilsAlt : public httpUtils_H
{

    public:
        string GET(string branch, string arch="") override
        {
            if(branch.empty()){
                cout << "branch name is empty" << endl;
                return "";
            } 
            url.append(branch);
            if(!arch.empty()){
                url.append("?arch=" + arch);
            }
            cout << url << endl;
            struct curl_slist *headers=NULL;
            headers = curl_slist_append(headers, "Accept: application/json");  
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charset: utf-8"); 
            CURL *curl = curl_easy_init();
            if(!curl){
                cout << "curl fail" << endl;
                curl_easy_cleanup(curl);
                return "";
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPGET,1); 
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);

            CURLcode responseCode = curl_easy_perform(curl);
            long http_code = 0;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
            
            if(responseCode != CURLE_OK){
                cout << "responseCode fail: " << responseCode << endl;
                cout << "http_code fail: " << http_code << endl;
                curl_easy_cleanup(curl);
                return "";
            }
            cout << "http_code: " << http_code << endl;

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return *DownloadedResponse;
        }

    private:
        
        string url = "https://rdb.altlinux.org/api/export/branch_binary_packages/";
        static string *DownloadedResponse;

        static size_t got_data(char *buffer, size_t itemSize, size_t nitems, string *buffer_in)
        {
            if(buffer_in != NULL){
                buffer_in->append(buffer, itemSize * nitems);
                DownloadedResponse = buffer_in;
                return itemSize * nitems;
            }
            return 0;            
        }
};