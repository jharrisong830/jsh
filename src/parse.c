/**
 * AUTHOR:  John Graham
 * 
 * FILE:    parse.c
*/

#include "parse.h"


/**
 * Reads from stdin and processes input into a supplied string array.
 * RETURN:
 *         0 ->     no errors
 *         1 ->     fgets error (most likely SIGINT during prompt, needs to be handled in jshell.c)
 *         2 ->     strtok immediately returns null, continue in jshell.c
 * MODIFIES: 
 *         char** string_array ->   places tokens within this array
 *         int* argc           ->   modifies the value of argc according to how many arguments are read in
*/
int get_tokens(char** string_array, int* argc) {
    char input[PATH_MAX];
    char* token;
    *argc=0;

    if(fgets(input, PATH_MAX, stdin)==NULL) { //read user input into x
        return 1; //return 1 to handle errors in jshell.c
    }

    input[strcspn(input, "\n")]='\0'; //replace newline with null terminator
    token=strtok(input, " "); //begin reading tokens, separating by space
    if(token==NULL) {
        return 2; //continue if no input supplied
    }

    while(token!=NULL) { //while there are still tokens to be read...
        string_array[*argc]=token; //add parsed result to the string array
        (*argc)++; //increment argc
        token=strtok(NULL, " "); //read the next token
    }
    string_array[*argc]=NULL; //add a null entry after the last token

    return 0;
}


/**
 * Correctly formats paths with spaces, also replacing '~' at the beginning if necessary. 
 * Allocates space for the full path as needed. Does NOT check for correctness of path or correct user supplied arguments
 * RETURN: a pointer to the full path, which must be freed later
*/
char* parse_cd(int argc, char** argv, const char* home) {
    char* fullpath;
    if(argv[1][0]=='~') { //allocate for and copy the home path, along with the remaining path
        fullpath=(char*)malloc(strlen(home)+strlen(argv[1])+1);
        strcpy(fullpath, home);
        strcat(fullpath, argv[1]+1);
    }
    else { //allocate for and copy the path at argv[1] (assume argv[0] is "cd")
        fullpath=(char*)malloc(strlen(argv[1])+1);
        strcpy(fullpath, argv[1]);
    }
    for(int i=2; i<argc; i++) { //for each following arg (which is assumed to be part of fullpath, separated by spaces)
        fullpath=(char*)realloc(fullpath, strlen(fullpath)+strlen(argv[i])+2); //realloc to accomodate the arg, along with an extra space
        strcat(fullpath, " "); //concat a space...
        strcat(fullpath, argv[i]); //...and the rest of the path
    }
    return fullpath; //return a pointer to the path (to be freed later)
}