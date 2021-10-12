#include <iostream>
#include <unistd.h>
#include <string>
#include <filesystem>
#include <sstream>
#include "perform_command.h"
#include <vector>
#include "util.h"

using namespace std;

string commands[3] = {"ls", "cd", "exit"};

string get_current_directory() {
    char dir[256];
    getcwd(dir, 256);
    string str_dir(dir);
    return str_dir;
}

vector<string> word_separation(string input) {
    string word;
    vector<string> words;
    stringstream ss(input);
    while (ss >> word) {
        words.push_back(word);
        // cout << word << endl;
    }
    return words;
}

void parse_arguments() {

}

bool parse_token(string token, string current_dir) {
    if (token == "ls") {
        char *inp = convert_string_to_char(current_dir);
        implement_ls(inp);
    } else if (token == "exit") {
        exit(1);
    } else {
        cout << "No valid token found" << endl;
    }
    return true;
}

int begin_ls(vector<string> tokens) {
    pid_t pid, wpid;
    int status;

    // char *inp = convert_string_to_char(tokens[0]);
    // vector<char *> pointerVec(tokens.size());
    // for (int i = 0; i < tokens.size(); i++) {
    //     pointerVec[i] = tokens[i].data();
    // }
    // char **inp_tokens = pointerVec.data();

    vector<char *> charVec(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        charVec[i] = &tokens[i][0];
    }

    char **inps = &charVec[0];

    // TODO: Fix multiple arguments
    // TODO: Fix bad address error sometimes

    // for(vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
    //     cout << *i << endl;
    // }

    pid = fork();
    if (pid == 0) {
        // This is the child process
        if (execvp(inps[0], inps) == -1) {
            perror("Error in ls");
        }
    } else if (pid < 0) {
        perror("Error in forking");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

bool is_keyword(string input, string current_dir) {
    char* c = strcpy(new char[current_dir.length() + 1], current_dir.c_str());
    char* c_i = strcpy(new char[input.length() + 1], input.c_str());
    vector<string> tokens = word_separation(input);
    // tokens.push_back(NULL);
    begin_ls(tokens);
    // if (input == "ls") {
    //     implement_ls(tokens.);
    // }
    // if (tokens.size() > 1) {
    //     parse_arguments();
    // }
    // cout << tokens[0] << endl;
    // cout << tokens[1] << endl;
    // parse_token(tokens[0], current_dir);
    // bool arguments = false;
    // for(vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
    //     if (!arguments) {
    //         arguments = true;
    //         continue;
    //     }
    //     parse_token(*i, current_dir);
    //     // for(int j = 0; j <= commands->size(); j++) {
    //     //     cout << commands[j] << endl;
    //     // }
    // }
    // string token;
    // for (token: tokens) {
    //     cout << token << endl;
    // }
    delete []c;
    return false;
}

void ctrlchandler(int signal) {
    cout << "Signal interrupted" << endl;
    exit(1);
}

void register_signal_handlers() {
    signal(SIGINT, ctrlchandler);
}

void begin_rsh() {
    // pid_t pid, wpid;
    // pid = fork();
    // if (pid == 0) {
    //     // This is the child process
    //     if (execvp())
    // }
}

int main(int argc, char *argv[]) {
    register_signal_handlers();
    string parent_dir = get_current_directory();
    cout << parent_dir << endl;
    do {
        string current_dir = get_current_directory();
        string dir = "";
        if (parent_dir == current_dir) {
            dir = "dir";
        } else {
            dir = current_dir;
        }
        cout << "[nyush " << dir << "] $ ";
        string text = "";
        std::getline(std::cin, text);
        if (text.empty()) {
            continue;
        }
        bool keyword = is_keyword(text, current_dir);
    } while(1);
    return 0;
}