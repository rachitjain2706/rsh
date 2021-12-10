#ifndef _UTIL_H_
#define _UTIL_H_

#include <cstring>
#include <string.h>
#include <iostream>

char* convert_string_to_char(std::string inp);

char **convert_vector_string_to_vector_char(std::vector<std::string> tokens);

std::string get_current_directory();

std::vector<std::string> tokenize(std::string input);

#endif