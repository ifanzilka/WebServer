#include <errno.h>		/* errno */
#include <string.h>
#include <iostream>
#include <sys/errno.h>
#include <string>
#include <sstream>
#include <vector>


/* This Function getting Error String from errno code */
std::string get_errno()
{
    char *str_error =  strerror(errno);
    std::cout << "Errno:" << errno << "\n";
    return (std::string(str_error));
}

int get_errno_code()
{
    return (errno);
}

std::vector<std::string> ft_split(std::string str, std::string delimiter)
{
    std::vector<std::string> parts;
    size_t pos = 0;
    std::string token;
    
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        parts.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    parts.push_back(str);
    return (parts);
}