#ifndef HTTPRESPONSE
# define HTTPRESPONSE

#include <sstream>      // std::ostringstream
#include <iostream>

class HttpResponse
{
public:
    HttpResponse();

    static std::string MakeHTTPRespone(int status, const std::string& content_type, const std::string& body);
private:
    static std::string get_status_text(int status);
};

#endif