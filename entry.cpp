#include <iostream>
#include <unistd.h>
#include <string>
#include <filesystem>
#include <sstream>
#include "perform_command.h"
#include <vector>
#include <sys/fcntl.h>
#include <csignal>
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
    int ppid = getpid();
    cout << "- " << ppid << endl;
    // exit(1);
}

void ctrldhandler(int signal) {
    int ppid = getpid();
    cout << "- " << ppid << endl;
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

void signal_handler_new_line(int signal) {
    cout << endl;
}

void register_signal_handlers() {
    signal(SIGINT, signal_handler_new_line);
    signal(SIGTSTP, signal_handler_new_line);
    signal(SIGQUIT, signal_handler_new_line);
    signal(SIGTERM, signal_handler_new_line);
}

int implement_cd(vector<string> tokens) {
    if (tokens[1].empty()) {
        cout << "No path given for cd" << endl;
    } else if (tokens.size() > 2) {
        cout << "Too many arguments for cd" << endl;
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
//    cout << "Parent process signal handler" << endl;
}

int implement_pipe(vector<string> tokens) {
    vector<vector<string> > pipe_vector(tokens.size());
    int j = 0;
    for (vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
        if (*i == "|") {
            j++;
//            cout << "Pipe operator" << endl;
            continue;
        }
//        cout << typeid(*i).name() << endl;
        pipe_vector[j].push_back(*i);
        cout << pipe_vector[j][0] << endl;
    }
    /*bool flag = true;
    vector<char *> arr1(tokens.size(), nullptr);
    vector<char *> arr2(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "|") {
            flag = false;
            arr1.push_back(NULL);
            j++;
            continue;
        }

        if(flag) {
            arr1[i] = &tokens[i][0];
            cout << "Arr1 - " << arr1[i] << endl;
        } else {
            arr2[i] = &tokens[i][0];
            cout << "Arr2 - " << arr2[i] << endl;
        }
    }
    cout << arr1.size() << endl;
    cout << arr2.size() << endl;
    cout << arr2[3] << endl;
    arr2.push_back(NULL);*/
//    char **inps = &arr1[0];

    int pipefd[2];
    int pid;

    return 1;
}

int implement_cat(vector<string> tokens) {
    /*in = open(inps[1], O_RDONLY);
    if (in == -1) {
//        cout << "File not present" << endl;
        return 0;
    }
    std::cout << in << std::endl;
    std::cout << out << std::endl;
    out = open("out.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

    dup2(in, 0);
    dup2(out, 1);

    close(in);
    close(out);*/
    int pid, wpid, status;
    pid = fork();
    if (pid == 0) {
        int in, out;
        vector<char *> charVec(tokens.size(), nullptr);
        string inp_path = "";
        string out_path = "";
        string out_append_path = "";
        bool flag1 = false, flag2 = false, flag3 = false;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] == "<") {
                if (tokens.size() == i + 1) {
                    cout << "No input params" << endl;
                    return 1;
                }
                inp_path = tokens[i + 1];
                flag1 = true;
                continue;
            }
            if (flag1) {
                flag1 = false;
                continue;
            }
            if (tokens[i] == ">") {
                out_path = tokens[i + 1];
                flag2 = true;
                continue;
            }
            if (flag2) {
                flag1 = false;
                continue;
            }
            if (tokens[i] == ">>") {
                out_append_path = tokens[i + 1];
                flag3 = true;
                continue;
            }
            if (flag3) {
                flag3 = false;
                continue;
            } else {
                charVec[i] = &tokens[i][0];
//            cout << charVec[i] << endl;
            }
        }
        charVec.push_back(NULL);

        char **inps = &charVec[0];
        if (!inp_path.empty()) {
            char *in_path = convert_string_to_char(inp_path);
            in = open(in_path, O_RDONLY);
            dup2(in, 0);
            close(in);
        }
        if (!out_path.empty()) {
            char *op_path = convert_string_to_char(out_path);
            out = open(op_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            dup2(out, 1);
            close(out);
        }

        if (!out_append_path.empty()) {
            char *op_path = convert_string_to_char(out_append_path);
            out = open(op_path, O_WRONLY | O_APPEND);
            dup2(out, 1);
            close(out);
        }
        if (execvp(inps[0], inps) == -1) {
            perror("Error in cat");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Error in forking");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
//    execvp(inps[0], inps);
    return 1;
}

int implement_other_commands(vector<string> tokens) {
    pid_t pid, wpid;
    int status;

//    char **inps = convert_vector_string_to_vector_char(tokens);
    vector<char *> charVec(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        charVec[i] = &tokens[i][0];
    }
    charVec.push_back(NULL);

    char **inps = &charVec[0];

    pid = fork();
    if (pid == 0) {
        // This is the child process
        // cout << "In child process" << endl;
//        register_signal_handlers();
//        pause();
        /*int ppid = getpid();
        cout << "Child - " << ppid << endl;*/
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        if (execvp(inps[0], inps) == -1) {
            perror("Error in ls");
        }
        /*signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);*/
        /*int ppid = getpid();
        cout << "Child - " << ppid << endl;*/
        exit(EXIT_FAILURE);
//        register_signal_handlers();
    } else if (pid < 0) {
//        perror("Error in forking");
    } else {
//        signal(SIGINT, reinterpret_cast<void (*)(int)>(noth));
//        pause();

        //do {
        wpid = waitpid(pid, &status, WUNTRACED);
//            int ppid = getpid();
//            cout << "Parent - " << ppid << " for - " << wpid << endl;
        //} while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

bool is_keyword(string input) {
    vector<string> tokens = word_separation(input);
    if (tokens[0] == "cd") {
        implement_cd(tokens);
//        cout << tokens[1] << endl;
    } else if (tokens[0] == "exit") {
        exit(1);
    } else if (tokens[0] == "job") {
//        cout << "JOB" << endl;
        exit(1);
    } else if (tokens[0] == "fg") {
//        cout << "FG" << endl;
        exit(1);
    } else if (std::find(tokens.begin(), tokens.end(), "<") != tokens.end()) {
        implement_cat(tokens);
//        cout << "Redirection" << endl;
    } else if (std::find(tokens.begin(), tokens.end(), ">") != tokens.end()) {
        implement_cat(tokens);
//        cout << "Redirection" << endl;
    } else if (std::find(tokens.begin(), tokens.end(), ">>") != tokens.end()) {
        implement_cat(tokens);
//        cout << "Redirection" << endl;
    } else if (std::find(tokens.begin(), tokens.end(), "|") != tokens.end()) {
        implement_pipe(tokens);
//        cout << "Piping" << endl;
    } else {
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
    return false;
}

int main(int argc, char *argv[]) {
    register_signal_handlers();
    string parent_dir = get_current_directory();
    do {
        string current_dir = get_current_directory();
        std::string delimiter = "/";
        size_t pos = 0;
        std::string token;
        while ((pos = current_dir.find(delimiter)) != std::string::npos) {
            token = current_dir.substr(0, pos);
            current_dir.erase(0, pos + delimiter.length());
        }
        string dir = "";
        dir = current_dir;
        cout << "[nyush " << dir << "] $ ";
        string text = "";
        std::getline(std::cin, text);
        if (cin.eof()) {
//            cout << "EOF cin" << endl;
            fflush(stdout);
            return 0;
        }
        if (text.empty()) {
            continue;
        }
        bool keyword = is_keyword(text);
        // if (keyword == false) {
        //     return 0;
        // }
    } while (1);
    return 0;
}