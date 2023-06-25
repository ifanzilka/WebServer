#include <errno.h>		/* errno */
#include <string.h>
#include <iostream>
#include <sys/errno.h>

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