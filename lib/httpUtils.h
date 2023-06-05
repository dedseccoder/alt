#include <curl/curl.h>
#include <string>

using std::string;

class httpUtils_H
{
    public:
        virtual void GET(string, string)=0;
};