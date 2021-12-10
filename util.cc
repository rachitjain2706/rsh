
#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include "cstring"

char* convert_string_to_char(std::string inp) {
    char* c = strcpy(new char[inp.length() + 1], inp.c_str());
    return c;
}

char **convert_vector_string_to_vector_char(std::vector<std::string> tokens) {
    std::vector<char *> charVec(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        charVec[i] = &tokens[i][0];
    }
    charVec.push_back(NULL);
    char **inps = &charVec[0];
    return inps;
}

std::string get_current_directory() {
    char dir[256];
    getcwd(dir, 256);
    std::string str_dir(dir);
    return str_dir;
}

std::vector<std::string> tokenize(std::string input) {
    std::string word;
    std::vector<std::string> words;
    std::stringstream ss(input);
    while (ss >> word) {
        words.push_back(word);
        // cout << word << endl;
    }
    return words;
}