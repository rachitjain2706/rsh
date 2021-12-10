#ifndef _PERFORM_COMMANDS_H_
#define _PERFORM_COMMANDS_H_

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <dirent.h>

void execute_io(std::vector<std::string> tokens);

void implement_ls(char* current_dir);

int implement_cd(std::vector<std::string> tokens);

int implement_pipe(std::vector<std::string> tokens);

int implement_mul_pipe(std::vector<std::string> tokens);

int implement_cat(std::vector<std::string> tokens);

int implement_other_commands(std::vector<std::string> tokens);

int implement_exit(std::vector<std::string> tokens);

int implement_jobs();

int implement_fg(std::vector<std::string> tokens);

#endif