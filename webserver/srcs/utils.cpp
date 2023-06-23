#include <errno.h>		/* errno */
#include <string.h>
#include <iostream>


/* This Function getting Error String from errno code */
std::string get_errno()
{
    char *str_error =  strerror(errno);
    return (std::string(str_error));
}