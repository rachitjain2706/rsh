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
#include "stdlib.h"
#include "cstring"

using namespace std;

string commands[4] = {"cd", "exit", "jobs", "fg"};

string get_current_directory() {
    char dir[256];
    getcwd(dir, 256);
    string str_dir(dir);
    return str_dir;
}

vector<vector<string>> cmds;
vector<int> jobs;
vector<bool> display;
int next_index = 0;

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

void signal_handler_new_line(int sgnl) {
    cout << endl;
    if (sgnl == SIGINT) {
        signal(SIGINT, signal_handler_new_line);
    } else if (sgnl == SIGTSTP) {
        signal(SIGTSTP, signal_handler_new_line);
    } else if (sgnl == SIGQUIT) {
        signal(SIGQUIT, signal_handler_new_line);
    } else if (sgnl == SIGTERM) {
        signal(SIGTERM, signal_handler_new_line);
    }
    fflush(stdout);
}

void register_signal_handlers() {
    signal(SIGINT, signal_handler_new_line);
    signal(SIGTSTP, signal_handler_new_line);
    signal(SIGQUIT, signal_handler_new_line);
    signal(SIGTERM, signal_handler_new_line);
}

int implement_cd(vector<string> tokens) {
    if (tokens[1].empty()) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
    } else if (tokens.size() > 2) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
    } else {
        vector<char *> charVec(tokens.size(), nullptr);
        for (int i = 0; i < tokens.size(); i++) {
            charVec[i] = &tokens[i][0];
        }
        charVec.push_back(NULL);

        char **inps = &charVec[0];
        if (chdir(inps[1]) != 0) {
//            perror("Error in cd");
            fprintf(stderr, "Error: invalid directory\n");
            fflush(stderr);
        }
    }
    return 1;
}

void noth() {
//    cout << "Parent process signal handler" << endl;
}

int implement_pipe(vector<string> tokens) {
    size_t num_pipes = std::count(tokens.begin(), tokens.end(), "|");
//    cout << num_pipes << endl;
    bool flag = false;
    vector<char *> charVec1(tokens.size(), nullptr);
    vector<char *> charVec2(tokens.size(), nullptr);
    int j = 0;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "|") {
            flag = true;
            charVec1.push_back(NULL);
            continue;
        }
        if (!flag) {
            charVec1[i] = &tokens[i][0];
        } else {
            charVec2[j] = &tokens[i][0];
            j++;
        }
    }
    charVec2.push_back(NULL);
    char **inps1 = &charVec1[0];
    char **inps2 = &charVec2[0];

    int pipefd[2];
    int pid, wpid, status;

    pipe(pipefd);
    pid = fork();

    if (pid == 0) {
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        if (execvp(inps2[0], inps2) == -1) {
            perror("Error in pipe");
        }
        cout << endl;
        fflush(stdout);
        exit(EXIT_FAILURE);
    } else {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        execvp(inps1[0], inps1);
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int implement_mul_pipe(vector<string> tokens) {
    size_t num_pipes = std::count(tokens.begin(), tokens.end(), "|");
    int numberOfPipes = 0;
//    cout << num_pipes << endl;
//    char **pipe_vector[num_pipes];
    int k = 0;
    vector<char **> piping;
    vector<vector<string>> pipe_vector;
    vector<string> tempCharVec;
    int j = 0;
    int ind = 0;
    for (const string &token: tokens) {
        if (token == "|") {
            if (ind == 0) {
                fprintf(stderr, "Error: invalid command\n");
                fflush(stderr);
                return 1;
            }
            numberOfPipes++;
            pipe_vector.push_back(tempCharVec);
            tempCharVec.clear();
            continue;
        }
        ind++;
        tempCharVec.push_back(token);
    }
    if (tempCharVec.empty()) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        return 1;
    }
    pipe_vector.push_back(tempCharVec);

    for (vector<string> &a: pipe_vector) {
        int k = 0;
        char **inp = static_cast<char **>(malloc(sizeof(char *) * (a.size() + 1)));
        for (auto &st: a) {
            char *inner_inp = static_cast<char *>(malloc((st.size() + 1) * sizeof(char)));
            std::strcpy(inner_inp, st.c_str());
            inner_inp[st.size()] = '\0';
            inp[k] = inner_inp;
            k++;
        }
        inp[k] = NULL;
        piping.push_back(inp);
    }

//    cout << piping.size() << endl;

//    cout << piping[0][0] << endl;
//    cout << piping[1][0] << endl;
//    cout << piping[1][1] << endl;

    /*for(const vector<string>& a : pipe_vector){
        for(const string& b: a){
            cout<< " " << b;
        }
        cout << endl;
    }*/

    /*for (vector<string> &a: pipe_vector) {
        vector<char *> charVec;
        charVec.reserve(a.size());
        for (auto &i: a) {
            charVec.push_back(&i[0]);
//            cout << &i[0] << " ";
        }
        charVec.emplace_back(nullptr);
        char **inputs = &charVec[0];
        piping.push_back(inputs);
    }*/

    /*for(vector<string> &a: pipe_vector) {
        vector<char*> pointerVec(a.size());
        for(unsigned i = 0; i < a.size(); ++i)
        {
            pointerVec[i] = a[i].data();
        } //you can use transform instead of this loop
        char** result = pointerVec.data();
    }*/

    /*for (vector<string> &a: pipe_vector) {
        std::vector<char *> charVec;
        for (int i = 0; i < a.size(); i++) {
            charVec[i] = a[i].c_str();
        }
        cout << charVec[0] << endl;
    }*/

//    cout << piping[0][0] << " - " << piping[1][0] << endl;

    /*for (const auto &command: pipe_vector) {
        int commandsArrayLength = 0;
        char **commandsArray;
        for (const auto &token: command) {
            int n = int(token.length());
            char commandArray[n + 1];
            strcpy(commandArray, token.c_str());
        }
        commandsArray[commandsArrayLength++] = commandsArray;
    }*/

    /*for (int k = 0; k < piping.size(); k++) {

        cout << *piping.at(k) << endl;
    }*/

    /*for(const vector<char *>& a : piping){
        for(const char& b: a){
            cout << " " << b;
        }
        cout << endl;
    }*/


//    for(const vector<string>& a : pipe_vector){
//        for(const string& b: a){
//            cout<< " " << b;
//        }
//        cout << endl;
//    }
//    cout << pipe_vector.at(0).at(0) << endl;
//    cout << pipe_vector.at(1).at(0) << endl;

//    for(int k = 0; k < 2*num_pipes; k++) {
//        char **inps1 = &pipe_vector[k][0];
//        piping.push_back(inps1);
//    }
    /*for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "|") {
            charVec1.push_back(NULL);
            char **inps1 = &charVec1[0];
//            piping.push_back(inps1);
//            pipe_vector[k] = inps1;
            pipe_vector.push_back(charVec1);
            k++;
            j = 0;
            vector<char *> charVec1(tokens.size(), nullptr);
            continue;
        }
        charVec1.push_back(&tokens[i][0]);*/
//        j++;
    //}

    int pipefd[2 * num_pipes];
    int pid, wpid, status;

    for (int i = 0; i < (num_pipes); i++) {
        if (pipe(pipefd + (i * 2)) < 0) {
            cout << endl;
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
    }

    int lp = 0;
    int commands = piping.size();
    int index = 0;
    while (commands != 0) {
        pid = fork();
        if (pid == 0) {

            if (index != piping.size() - 1) {
                if (dup2(pipefd[index * 2 + 1], 1) < 0) {
                    cout << "DUP2 output" << endl;
                    cout << endl;
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }
            }
//            cout << "DUP2 working output " << pipefd[index*2 + 1] << " with index - " << index << endl;

            if (index != 0) {
                /*if (dup2(pipefd[(index - 1)*2], 0) < 0) {
                    cout << "DUP2 input " << pipefd[(index - 1)*2] << " with index - " << index << endl;
                    exit(EXIT_FAILURE);
                }*/
                dup2(pipefd[(index - 1) * 2], 0);
            }
//            cout << "DUP2 working input " << pipefd[(index - 1)*2] << " with index - " << index << endl;



            for (int x = 0; x < 2 * num_pipes; x++) {
                close(pipefd[x]);
            }
            if (execvp(piping[index][0], piping[index]) < 0) {
                cout << "Error in execvp pipe" << endl;
                cout << endl;
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            cout << "Error in forking" << endl;
            cout << endl;
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
        commands--;
        index++;
    }

    for (int i = 0; i < 2 * num_pipes; i++) {
        close(pipefd[i]);
    }
    for (int i = 0; i < num_pipes + 1; i++) {
        wait(&status);
    }


    /*if (pid == 0) {
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        if (execvp(piping[1][0], piping[1]) == -1) {
            perror("Error in pipe");
        }
        cout << endl;
        fflush(stdout);
        exit(EXIT_FAILURE);
    } else {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        execvp(piping[0][0], piping[0]);
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }*/

    return 1;
}

/*int implement_pipe(vector<string> tokens) {
    vector<char **> pipe_vector(tokens.size());
    int j = 0;
    *//*vector<char *> charVec(tokens.size(), nullptr);
    for (vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
        if (*i == "|") {
            charVec.push_back(NULL);
//            cout << *charVec << endl;
            char **inps = &charVec[0];
            cout << inps[0] << endl;
            pipe_vector.push_back(inps);
            cout << pipe_vector[1] << endl;
            j++;
            vector<char *> charVec(tokens.size(), nullptr);
            continue;
        }
//        cout << typeid(*i).name() << endl;
        char *val = convert_string_to_char(*i);
//        cout << "Value - " << val << endl;

        charVec.push_back(val);
//        cout << charVec[]
//        cout << typeid(charVec).name() << endl;
//        pipe_vector[j].push_back(charVec);
//        cout << pipe_vector[j][0] << endl;
    }
//    cout << pipe_vector[0][0] << endl;
    fflush(stdout);*//*

    vector<char *> charVec(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "|") {
            j++;
            charVec.push_back(NULL);
            char **inps = &charVec[0];
            pipe_vector.push_back(inps);
            continue;
        }
        charVec[i] = &tokens[i][0];
//        cout << charVec[i] << endl;
    }
    cout << pipe_vector[0][0] << endl;

    *//*int in, out;
    vector<char *> charVec(tokens.size(), nullptr);
    string inp_path = "";
    bool flag1 = false, flag2 = false, flag3 = false;
    for (int k = 0; k <= j; k++) {
        for (int i = 0; i < pipe_vector[k].size(); i++) {
            charVec[i] = &pipe_vector[k][i][0];
        }
        charVec.push_back(NULL);
    }

    char **inps = &charVec[0];*//*

    int pipefd[2];
    int pid;

    pipe(pipefd);

    pid = fork();

    if (pid == 0) {
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        execvp(pipe_vector[0][0], pipe_vector[0]);
    } else {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        execvp(pipe_vector[1][0], pipe_vector[1]);
    }
    *//*bool flag = true;
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
    arr2.push_back(NULL);*//*
//    char **inps = &arr1[0];

    return 1;
}*/

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
                    fprintf(stderr, "Error: invalid command\n");
                    fflush(stderr);
                    return 1;
                }
                inp_path = tokens[i + 1];
                if (access(convert_string_to_char(inp_path), F_OK) != 0) {
                    fprintf(stderr, "Error: invalid file\n");
                    fflush(stderr);
                    return 1;
                }
                flag1 = true;
                continue;
            }
            if (flag1) {
                flag1 = false;
                continue;
            }
            if (tokens[i] == ">") {
                if (tokens.size() == i + 1) {
                    fprintf(stderr, "Error: invalid command\n");
                    fflush(stderr);
                    return 1;
                }
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
        cout << endl;
        fflush(stdout);
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
    int cpid;
    if (pid == 0) {
        // This is the child process
        // cout << "In child process" << endl;
//        register_signal_handlers();
//        pause();
        cpid = getpid();
        /*cout << "Child - " << ppid << endl;*/
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        if (execvp(inps[0], inps) == -1) {
//            perror("Error: Invalid command");
            fprintf(stderr, "Error: invalid command");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        /*signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);*/
        /*int ppid = getpid();
        cout << "Child - " << ppid << endl;*/
        cout << endl;
        fflush(stdout);
        exit(EXIT_FAILURE);
//        register_signal_handlers();
    } else if (pid < 0) {
//        perror("Error in forking");
//        exit()
    } else {
//        signal(SIGINT, reinterpret_cast<void (*)(int)>(noth));
//        pause();
        //do {
        wpid = waitpid(pid, &status, WUNTRACED);
//        cout << status << endl;
        if (status < 0) {
//            cout << "";
//            exit(EXIT_FAILURE);
        }
        if (WIFSTOPPED(status)) {
            cout << "Process stopped - " << pid << endl;
            cmds.push_back(tokens);
            jobs.push_back(pid);
            display.push_back(true);
            next_index++;
        }
//            int ppid = getpid();
//            cout << "Parent - " << ppid << " for - " << wpid << endl;
        //} while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int implement_exit(vector<string> tokens) {
    if (tokens.size() > 1) {
//        cout << "Error: Invalid command" << endl;
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        return 1;
    }
    for (int i = 0; i < cmds.size(); i++) {
        if (display[i]) {
//            cout << "Error: there are suspended jobs" << endl;
            fprintf(stderr, "Error: there are suspended jobs\n");
            fflush(stderr);
            return 1;
        }
    }
    cout << endl;
    fflush(stdout);
    exit(1);
}

int implement_jobs() {
    for (int i = 0; i < next_index; i++) {
        if (display[i]) {
            cout << "[" << (i + 1) << "] " << cmds[i][0] << endl;// << " - " << jobs[i];
            fflush(stdout);
        }
    }
    return 1;
}

int implement_fg(vector<string> tokens) {
    if (tokens.size() != 2) {
//        cout << "Error: Invalid command" << endl;
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        return 1;
//        exit(EXIT_FAILURE);
    }
    int index = stoi(tokens[1]);
    if (index < 0) {
        fprintf(stderr, "Error: invalid job\n");
        fflush(stderr);
        return 1;
//        exit(EXIT_FAILURE);
    }
    int len = 0;
    for (int i = 0; i < next_index; i++) {
        if (display[i]) {
            len++;
        }
    }
    if (len <= 0 || (len < index - 1) || index == 0) {
        fprintf(stderr, "Error: invalid job\n");
        fflush(stderr);
        return 1;
//        exit(EXIT_FAILURE);
    }
//    cmds.erase(std::remove(cmds.begin(), cmds.end(), index-1), cmds.end());
//    cmds.erase(std::next(cmds.begin(), index-1), std::next(cmds.begin(), index));
//    jobs.erase(std::next(jobs.begin(), index-1), std::next(jobs.begin(), index));
//    jobs.erase(std::remove(jobs.begin(), jobs.end(), index-1), jobs.end());
    display[index - 1] = false;
    return 1;
}

bool is_keyword(string input) {
    vector<string> tokens = word_separation(input);
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
    } else if (std::find(tokens.begin(), tokens.end(), "<") != tokens.end()) {
        implement_cat(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), ">") != tokens.end()) {
        implement_cat(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), ">>") != tokens.end()) {
        implement_cat(tokens);
    } else if (std::find(tokens.begin(), tokens.end(), "|") != tokens.end()) {
        implement_mul_pipe(tokens);
    } else {
        implement_other_commands(tokens);
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
        cout << "[nyush " << dir << "]$ ";
        fflush(stdout);
        string text = "";
        std::getline(std::cin, text);
        if (cin.eof()) {
            cout << endl;
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
    cout << endl;
    fflush(stdout);
    return 0;
}