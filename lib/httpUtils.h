#include <curl/curl.h>
#include <string>

using std::string;

class httpUtils_H
{
    public:
        virtual string GET(string, string)=0;
};