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
#include <dirent.h>

#include "util.h"

std::string commands[4] = {"cd", "exit", "jobs", "fg"};

std::vector<std::vector<std::string>> cmds;
std::vector<int> jobs;
std::vector<bool> display;
int next_index = 0;

void execute_io(std::vector<std::string> tokens) {
    int in, out;
    std::vector<char *> charVec(tokens.size(), nullptr);
    std::string inp_path = "";
    std::string out_path = "";
    std::string out_append_path = "";
    bool flag1 = false, flag2 = false, flag3 = false;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "<") {
            if (tokens.size() == i + 1) {
                fprintf(stderr, "Error: invalid command\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            if (flag1) {
                fprintf(stderr, "Error: invalid command\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            inp_path = tokens[i + 1];
            if (access(convert_string_to_char(inp_path), F_OK) != 0) {
                fprintf(stderr, "Error: invalid file\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            if (tokens.size() > i + 2 && tokens[i + 2] != ">" && tokens[i + 2] != ">>") {
                fprintf(stderr, "Error: invalid command\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
            }
            if (flag2) {
                fprintf(stderr, "Error: invalid command\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            out_path = tokens[i + 1];
            if (tokens.size() > i + 2) {
                fprintf(stderr, "Error: invalid command\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
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
//            std::cout << charVec[i] << std::endl;
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
    int status = execvp(inps[0], inps);
    if (status < 0) {
        if (strncmp(inps[0], "/usr/bin/", strlen("/usr/bin/")) == 0) {
            char * token, * last;
            last = token = strtok(inps[0], "/");
            for (;(token = strtok(NULL, "/")) != NULL; last = token);
            if (last != nullptr) {
                if (execvp(last, inps) == -1) {
                    fprintf(stderr, "Error: invalid program 1\n");
                    fflush(stderr);
                    exit(EXIT_FAILURE);
                }
            }
        }
        fprintf(stderr, "Error: invalid program\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    std::cout << std::endl;
    fflush(stdout);
    exit(EXIT_FAILURE);
}

void implement_ls(char* current_dir) {
    DIR *dir;
    struct dirent *ent;
    //char* c = strcpy(new char[current_dir.length() + 1], current_dir.c_str());
    if ((dir = opendir(current_dir)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::cout <<  ent->d_name << "\t";
        }
        std::cout << std::endl;
        closedir (dir);
    } else {
        std::cout << "No files" << std::endl;
    }
    // delete []c;
}

int implement_cd(std::vector<std::string> tokens) {
    if (tokens.size() == 1 || tokens[1].empty()) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
    } else if (tokens.size() > 2) {
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
    } else {
        std::vector<char *> charVec(tokens.size(), nullptr);
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

int implement_pipe(std::vector<std::string> tokens) {
    size_t num_pipes = std::count(tokens.begin(), tokens.end(), "|");
//    std::cout << num_pipes << std::endl;
    bool flag = false;
    std::vector<char *> charVec1(tokens.size(), nullptr);
    std::vector<char *> charVec2(tokens.size(), nullptr);
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
        std::cout << std::endl;
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

int implement_mul_pipe(std::vector<std::string> tokens) {
    size_t num_pipes = std::count(tokens.begin(), tokens.end(), "|");
    int numberOfPipes = 0;
    int k = 0;
    std::vector<char **> piping;
    std::vector<std::vector<std::string>> pipe_vector;
    std::vector<std::string> tempCharVec;
    int j = 0;
    int ind = 0;
    for (const std::string &token: tokens) {
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

    for (std::vector<std::string> &a: pipe_vector) {
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

    int pipefd[2 * num_pipes];
    int pid, wpid, status;

    for (int i = 0; i < (num_pipes); i++) {
        if (pipe(pipefd + (i * 2)) < 0) {
            std::cout << std::endl;
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

            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTERM, SIG_DFL);

            if (index != piping.size() - 1) {
                if (dup2(pipefd[index * 2 + 1], 1) < 0) {
                    fprintf(stderr, "Error: invalid program\n");
                    fflush(stderr);
                    exit(EXIT_FAILURE);
                }
            }

            if (index != 0) {
                if (dup2(pipefd[(index - 1)*2], 0) < 0) {
                    fprintf(stderr, "Error: invalid program\n");
                    fflush(stderr);
                    exit(EXIT_FAILURE);
                }
            }

            for (int x = 0; x < 2 * num_pipes; x++) {
                close(pipefd[x]);
            }

            execute_io(pipe_vector[index]);
            
        } else if (pid < 0) {
            std::cout << "Error in forking" << std::endl;
            std::cout << std::endl;
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
        if (WIFSTOPPED(status)) {
            cmds.push_back(tokens);
            jobs.push_back(pid);
            display.push_back(true);
            next_index++;
        }
    }

    return 1;
}

int implement_cat(std::vector<std::string> tokens) {
    int pid, wpid, status;
    pid = fork();
    if (pid == 0) {
        execute_io(tokens);
    } else if (pid < 0) {
        perror("Error in forking");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (WIFSTOPPED(status)) {
                std::cout << "Process stopped - " << pid << std::endl;
                cmds.push_back(tokens);
                jobs.push_back(pid);
                display.push_back(true);
                next_index++;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
//    execvp(inps[0], inps);
    return 1;
}


int implement_other_commands(std::vector<std::string> tokens) {
    pid_t pid, wpid;
    int status;

//    char **inps = convert_vector_string_to_vector_char(tokens);
    std::vector<char *> charVec(tokens.size(), nullptr);
    for (int i = 0; i < tokens.size(); i++) {
        charVec[i] = &tokens[i][0];
    }
    charVec.push_back(NULL);

    char **inps = &charVec[0];

    pid = fork();
    int cpid;
    if (pid == 0) {
        // This is the child process
       
        cpid = getpid();
        /*std::cout << "Child - " << ppid << std::endl;*/
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        if (execvp(inps[0], inps) == -1) {
            if (strncmp(inps[0], "/usr/bin/", strlen("/usr/bin/")) == 0) {
                char * token, * last;
                last = token = strtok(inps[0], "/");
                for (;(token = strtok(NULL, "/")) != NULL; last = token);
                if (last != nullptr) {
                    if (execvp(last, inps) == -1) {
                        fprintf(stderr, "Error: invalid program 1\n");
                        fflush(stderr);
                        exit(EXIT_FAILURE);
                    }
                }
            }
//            perror("Error: Invalid command");
            fprintf(stderr, "Error: invalid program\n");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        std::cout << std::endl;
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
//        std::cout << status << std::endl;
        if (status < 0) {
//            std::cout << "";
//            exit(EXIT_FAILURE);
        }
        if (WIFSTOPPED(status)) {
            cmds.push_back(tokens);
            jobs.push_back(pid);
            display.push_back(true);
            next_index++;
        }
//            int ppid = getpid();
//            std::cout << "Parent - " << ppid << " for - " << wpid << std::endl;
        //} while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int implement_exit(std::vector<std::string> tokens) {
    if (tokens.size() > 1) {
//        std::cout << "Error: Invalid command" << std::endl;
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        return 1;
    }
    for (int i = 0; i < cmds.size(); i++) {
        if (display[i]) {
            fprintf(stderr, "Error: there are suspended jobs\n");
            fflush(stderr);
            return 1;
        }
    }
//    std::cout << std::endl;
    fflush(stdout);
    exit(1);
}

int implement_jobs() {
    for (int i = 0; i < next_index; i++) {
        if (display[i]) {
            std::cout << "[" << (i + 1) << "] " << cmds[i][0] << std::endl;// << " - " << jobs[i];
            fflush(stdout);
        }
    }
    return 1;
}

int implement_fg(std::vector<std::string> tokens) {
    if (tokens.size() != 2) {
//        std::cout << "Error: Invalid command" << std::endl;
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
    if (len <= 0 || (len < index - 1) || index == 0 || !display[index - 1]) {
        fprintf(stderr, "Error: invalid job\n");
        fflush(stderr);
        return 1;
    }
    display[index - 1] = false;
    kill(jobs[index-1], SIGCONT);
    int status = 0;
    int wpid = waitpid(jobs[index-1], &status, WUNTRACED);
    if (WIFSTOPPED(status)) {
        jobs[index - 1] = wpid;
        display[index - 1] = true;
    }
    return 1;
}