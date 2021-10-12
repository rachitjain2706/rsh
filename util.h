#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <iostream>

using namespace std;

char* convert_string_to_char(string inp) {
    char* c = strcpy(new char[inp.length() + 1], inp.c_str());
    return c;
}

#endif