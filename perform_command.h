#ifndef _PERFORM_COMMAND_H_
#define _PERFORM_COMMAND_H_

#include <iostream>
#include <string>
#include <filesystem>
#include <dirent.h>

using namespace std;

void implement_ls(char* current_dir) {
    DIR *dir;
    struct dirent *ent;
    //char* c = strcpy(new char[current_dir.length() + 1], current_dir.c_str());
    if ((dir = opendir(current_dir)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            cout <<  ent->d_name << "\t";
        }
        cout << endl;
        closedir (dir);
    } else {
        cout << "No files" << endl;
    }
    // delete []c;
}

#endif