#ifndef HTTP_REQUESTS
# define HTTP_REQUESTS

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "utils.hpp"


struct HttpRequest
{
    std::string                 method;
    std::string                 path;
    std::string                 version;
    std::vector<std::string>    headers;
    std::string                 body;
};

class HttpRequestParser
{
public:
    HttpRequest parse(std::string request) {
        HttpRequest http_request;

        // Split the request into lines
        std::vector<std::string> lines = ft_split(request, "\r\n");

        // Parse the first line
        std::vector<std::string> parts = ft_split(lines[0], " ");
        http_request.method = parts[0];
        http_request.path = parts[1];
        http_request.version = parts[2];

        // Parse the headers
        for (int i = 1; i < lines.size(); i++) {
            http_request.headers.push_back(lines[i]);
        }

        // Parse the body
        int body_index = request.find("\r\n\r\n");
        if (body_index != std::string::npos) {
            http_request.body = request.substr(body_index + 4);
        }

        return http_request;
    }

private:
};

#endif