#include <iostream>
#include <unistd.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
#include <sys/fcntl.h>
#include "stdlib.h"
#include "cstring"
#include <algorithm>
#include <sys/wait.h>
#include <signal.h>

#include "util.h"
#include "perform_commands.h"

void signal_suppressor(int sgnl) {
    std::cout << std::endl;
    fflush(stdout);
}

void register_signal_handlers() {
    signal(SIGINT, signal_suppressor);
    signal(SIGTSTP, signal_suppressor);
    signal(SIGQUIT, signal_suppressor);
    signal(SIGTERM, signal_suppressor);
}

bool execute(std::string input) {

    char *x = static_cast<char *>(malloc(sizeof(char) * (input.size() + 1)));
    std::strcpy(x, input.c_str());
    char *first = strtok_r(x, "|", &x);
    int ind = 0;
    int oi = 100000;
    std::vector<char *> ins;
    while (first && strlen(first) > 0 && strcmp(first, "\n") != 0 && strcmp(first, " ") != 0) {
        ins.push_back(first);
        if (strstr(first, "<") != NULL && ind != 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stderr);
            return false;
        }

        if (strstr(first, ">") != NULL || strstr(first, ">>") != NULL) {
            oi = std::min(oi, ind);
        }
        first = strtok_r(x, "|", &x);
        ind++;
    }
    if (ind - 1 != oi && oi != 100000) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        return false;
    }

    std::vector<std::string> tokens = tokenize(input);
    if (tokens[0] == "cd") {
        implement_cd(tokens);
    } else if (tokens[0] == "exit") {
        implement_exit(tokens);
    } else if (tokens[0] == "jobs") {
        implement_jobs();
    } else if (tokens[0] == "fg") {
        implement_fg(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), "<<") != tokens.end()) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        return false;
    } else if (std::find(tokens.begin(), tokens.end(), "|") != tokens.end()) {
        implement_mul_pipe(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), "<") != tokens.end()) {
        implement_cat(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), ">") != tokens.end()) {
        implement_cat(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), ">>") != tokens.end()) {
        implement_cat(tokens);
    } else {
        implement_other_commands(tokens);
    }
    return false;
}

int main(int argc, char *argv[]) {
    register_signal_handlers();
    std::string parent_dir = get_current_directory();
    do {
        std::string current_dir = get_current_directory();
        std::string delimiter = "/";
        size_t pos = 0;
        std::string token;
        while ((pos = current_dir.find(delimiter)) != std::string::npos) {
            token = current_dir.substr(0, pos);
            current_dir.erase(0, pos + delimiter.length());
        }
        std::string dir = "";
        dir = current_dir;
        std::cout << "[nyush " << dir << "]$ ";
        fflush(stdout);
        std::string text = "";
        std::getline(std::cin, text);
        if (std::cin.eof()) {
            std::cout << std::endl;
            fflush(stdout);
            return 0;
        }
        if (text.empty()) {
            continue;
        }
        execute(text);
    } while (true);
    std::cout << std::endl;
    fflush(stdout);
    return 0;
}