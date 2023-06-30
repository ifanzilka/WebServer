#ifndef HTTPRESPONSE
# define HTTPRESPONSE


#ifndef BUFFER_SIZE_SEND
# define BUFFER_SIZE_SEND 4096
#endif

#include <sstream>      // std::ostringstream
#include <iostream>
#include <fstream>      //ifstream
#include <sys/socket.h> //send
#include <sys/errno.h>

int send_bytes_fd(int fd, const char *msg, int size);

class HttpResponse
{
public:
    HttpResponse();

    static std::string  MakeHTTPResponse(int status, const std::string& content_type, const std::string& body);
    static std::string  MakeHTTPResponse(int status, const std::string& content_type,  int size);
    static int          SendHTTPResponse(int fd, int status, const std::string& content_type,  char *str);
    static int          SendHTTPResponseFile(int fd, int status, const std::string& content_type, const std::string& filename);
    
    static std::string  SendBytesHTTPResponse(int fd, const std::string& filename);
    
private:
    static std::ostringstream  set_header(std::ostringstream &response , const std::string& key, const std::string& value);
    static std::string  set_header(std::string &response , const std::string& key, const std::string& value);
    static std::string  get_status_text(int status);
};


#endif