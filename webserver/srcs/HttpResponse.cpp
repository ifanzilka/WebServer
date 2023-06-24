#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
{

} 

std::string HttpResponse::MakeHTTPRespone(int status, const std::string& content_type, const std::string& body)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << get_status_text(status) << "\r\n";
    oss << "Content-Type: " << content_type << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "\r\n";
    oss << body;
    std::string response_str = oss.str();
    //sendall(sockfd, response_str.c_str(), response_str.size());
    return (response_str);
}

std::string HttpResponse::get_status_text(int status)
{
    switch (status)
    {
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
        default:
            return "Unknown Status";
    }
}