/**
 * AUTHOR:  John Graham
 * 
 * FILE:    directory.c
*/

#include "directory.h"


int list_dir(char* path) {
    DIR* directory;
    if((directory=opendir(path)) == NULL) { //open directory
        return -1;
    }
    struct dirent* curr_ent;
    struct stat curr_stat;
    while((curr_ent=readdir(directory)) != NULL) { //read from each entry
        if(curr_ent->d_name[0]!='.') { //skip all hidden entries
            stat(curr_ent->d_name, &curr_stat);
            if(S_ISDIR(curr_stat.st_mode)) { //if directory, print in green
                fprintf(stdout, "%s%s%s\n", GREEN, curr_ent->d_name, DEFAULT);
            }
            else { //otherwise, print in default color
                fprintf(stdout, "%s\n", curr_ent->d_name);
            }
        }
    }
    free(directory);
    return 0;
}