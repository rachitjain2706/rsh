#include <iostream>
#include <unistd.h>
#include <string>
#include <filesystem>
#include <sstream>
#include "perform_command.h"
#include <vector>
#include "util.h"

using namespace std;

string commands[4] = {"cd", "exit", "jobs", "fg"};

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

void ctrlchandler(int signal) {
    cout << "Signal interrupted" << endl;
    // exit(1);
}

void ctrldhandler(int signal) {
    cout << "Signal stopped" << endl;
    // exit(1);
}

void ctrlqhandler(int signal) {
    cout << "Signal quit" << endl;
    // exit(1);
}

void ctrlthandler(int signal) {
    cout << "Signal term" << endl;
    // exit(1);
}

void register_signal_handlers() {
    signal(SIGINT, ctrlchandler);
    signal(SIGTSTP, ctrldhandler);
    signal(SIGQUIT, ctrlqhandler);
    signal(SIGTERM, ctrlthandler);
}

int implement_cd(vector<string> tokens) {
    if (tokens[1].empty()) {
        cout << "No path given for cd";
    } else {
        vector<char *> charVec(tokens.size(), nullptr);
    
        for (int i = 0; i < tokens.size(); i++) {
            charVec[i] = &tokens[i][0];
        }
        charVec.push_back(NULL);

        char **inps = &charVec[0];
        if (chdir(inps[1]) != 0) {
            perror("Error in cd");
        }
    }
    return 1;
}

void noth() {
    cout << "Parent process signal handler" << endl;
}

int implement_other_commands(vector<string> tokens) {
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
    charVec.push_back(NULL);

    char **inps = &charVec[0];

    // for(vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
    //     cout << *i << endl;
    // }

    pid = fork();
    if (pid == 0) {
        // This is the child process
        register_signal_handlers();
//        signal(SIGINT, ctrlchandler);
        if (execvp(inps[0], inps) == -1) {
            perror("Error in ls");
        }
    } else if (pid < 0) {
        perror("Error in forking");
    } else {
        signal(SIGINT, reinterpret_cast<void (*)(int)>(noth));
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
    if (tokens[0] == "cd") {
        char *inp = convert_string_to_char(current_dir);
        implement_cd(tokens);
        cout << tokens[1] << endl;
    } else if (tokens[0] == "exit") {
        exit(1);
    } else if(tokens[0] == "job") {
        cout << "JOB" << endl;
        exit(1);
    } else if(tokens[0] == "fg") {
        cout << "FG" << endl;
        exit(1);
    } /*else if (tokens[0] == "cat") {
        implement_cat(tokens);
    } */else {
        implement_other_commands(tokens);
        //cout << "No valid token found" << endl;
    }
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

int main(int argc, char *argv[]) {
//    register_signal_handlers();
    string parent_dir = get_current_directory();
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
        // if (keyword == false) {
        //     return 0;
        // }
    } while(1);
    return 0;
}