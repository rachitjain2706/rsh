#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <iostream>

using namespace std;

char* convert_string_to_char(string inp) {
    char* c = strcpy(new char[inp.length() + 1], inp.c_str());
    return c;
}

char **convert_vector_string_to_vector_char(vector<string> tokens) {
    vector<char *> charVec(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        charVec[i] = &tokens[i][0];
    }
    charVec.push_back(NULL);
    char **inps = &charVec[0];
    return inps;
}

#endif