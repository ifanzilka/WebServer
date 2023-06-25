#ifndef UTILS_HPP
# define UTILS_HPP

#include <string.h>
#include <iostream>

std::string get_errno();
int         get_errno_code();


std::vector<std::string> ft_split(std::string str, std::string delimiter);
#endif 