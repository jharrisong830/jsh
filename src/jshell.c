/**
 * AUTHOR:  John Graham
 * 
 * FILE:    jshell.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "parse.h"
#include "directory.h"
#include "color.h"


volatile int interrupt=0;
char homedir[PATH_MAX];

void sig_handler(int sig_num) { //signal handler, sets interrupt to true, prints newline
    interrupt=1;
    fprintf(stdout, "\n");
}


int prompt() { //function that loops to
    struct sigaction signal;
    signal.sa_handler=sig_handler;
    sigaction(SIGINT, &signal, NULL); //installing the signal handler to capture SIGINT (ctrl+C)

    char x[PATH_MAX]; //for reading input
    char* cwd=getcwd(NULL, 0); //for printing cwd to the prompt
    char* old_cwd;
    char* parse_array[PATH_MAX]; //string array to hold each all of the tokens together
    int argc;
    pid_t pid; //holds the process ID number

    while(1) { //loop for the shell
        interrupt=0; //reset interrupt to false

        /* RECEIVING INPUT */
        printf("%s[%s]%s> ", BLUE, cwd, DEFAULT); //prints the shell in blue
        int parse_status=get_tokens(parse_array, &argc);
        if(parse_status==1) { //some error with fgets(), handle accordingly
            if(interrupt) { //don't print error if interrupted by SIGINT
                interrupt=0;
            }
            else {
                fprintf(stderr, "%sError: Failed to read from stdin. %s.%s\n", RED, strerror(errno), DEFAULT);
            }
            continue;
        }
        else if(parse_status==2) { //no input (just whitespace), continue
            continue;
        }

        /* COMMANDS */
        if(strcmp(parse_array[0], "exit")==0) { //exit command
            free(cwd);
            exit(EXIT_SUCCESS); //exit the loop and end the program
        }
        else if(strcmp(parse_array[0], "cd")==0) { //cd command
            if(argc==1 || strcmp(parse_array[1], "~")==0) { //if no other arguments supplied, or given '~'...
                if(chdir(homedir)!=0) { //change to the user's home directory
                    fprintf(stderr, "%sError: Cannot change directory to \"%s\". %s.%s\n", RED, homedir, strerror(errno), DEFAULT);
                    continue;
                }
            }
            else {
                char* targetpath=parse_cd(argc, parse_array, homedir); //resolve '~' and spacing issues
                if(chdir(targetpath)!=0) { //change to desired path
                    fprintf(stderr, "%sError: Cannot change directory to \"%s\". %s.%s\n", RED, targetpath, strerror(errno), DEFAULT);
                    free(targetpath);
                    continue;
                }
                free(targetpath);
            }
            free(cwd); //free the old cwd...
            if((cwd=getcwd(NULL, 0)) == NULL) { //...and set the new one
                fprintf(stderr, "%sError: Cannot get current working directory. %s.%s\n", RED, strerror(errno), DEFAULT);
                continue;
            }
        }
        else if(strcmp(parse_array[0], "ls")==0) { //colorized ls command
            if(argc==1) { //if no other arguments supplied, print contents of current directory
                if(list_dir(cwd)==-1) {
                    fprintf(stderr, "%sError: opendir() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                    continue;
                }
            }
            else {
                for(int i=1; i<argc; i++) {
                    if(strcmp(parse_array[i], "~")==0) { //if given '~'...
                        if(chdir(homedir)!=0) { //change to the user's home directory
                            fprintf(stderr, "%sError: Cannot change directory to %s. %s.%s\n", RED, homedir, strerror(errno), DEFAULT);
                            continue;
                        }
                        parse_array[i]=homedir; //change '~' to the user's home directory path
                    }
                    else { //otherwise, change to the directory supplied 
                        if(chdir(parse_array[i])!=0) {
                            fprintf(stderr, "%sError: Cannot change directory to %s. %s.%s\n", RED, parse_array[1], strerror(errno), DEFAULT);
                            continue;
                        }
                    }
                    if((old_cwd=(char*)malloc(strlen(cwd)+1)) == NULL) { //save the contents of the old cwd for later
                        fprintf(stderr, "%sError: malloc() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                        continue;
                    }
                    strcpy(old_cwd, cwd);
                    free(cwd); //free the old directory...
                    if((cwd=getcwd(NULL, 0)) == NULL) { //...and set the new one
                        fprintf(stderr, "%sError: Cannot get current working directory. %s.%s\n", RED, strerror(errno), DEFAULT);
                        continue;
                    }
                    if(argc>2) { //if printing more than one directory, print the current directory's name
                        fprintf(stdout, "%s%s%s:\n", B_GREEN, cwd, DEFAULT);
                    }
                    if(list_dir(cwd)==-1) { //open directory
                        fprintf(stderr, "%sError: opendir() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                        if(chdir(old_cwd)!=0) { //change back to cwd
                            fprintf(stderr, "%sError: Cannot change directory to %s. %s.%s\n", RED, cwd, strerror(errno), DEFAULT);
                            continue;
                        }
                        free(old_cwd);
                        free(cwd); //free the old directory...
                        if((cwd=getcwd(NULL, 0)) == NULL) { //...and set the new one
                            fprintf(stderr, "%sError: Cannot get current working directory. %s.%s\n", RED, strerror(errno), DEFAULT);
                            continue;
                        }
                        continue;
                    }
                    if(chdir(old_cwd)!=0) { //change back to cwd
                        fprintf(stderr, "%sError: Cannot change directory to %s. %s.%s\n", RED, cwd, strerror(errno), DEFAULT);
                        continue;
                    }
                    free(old_cwd);
                    free(cwd); //free the old directory...
                    if((cwd=getcwd(NULL, 0)) == NULL) { //...and set the new one
                        fprintf(stderr, "%sError: Cannot get current working directory. %s.%s\n", RED, strerror(errno), DEFAULT);
                        continue;
                    }
                    if(argc>2 && i!=(argc-1)) { //print an extra newline if we are printing multiple directories, and we're not at the last directory
                        fprintf(stdout, "\n");
                    }
                }
            }
        }
        else { //all other commands
            int stat;
            pid=fork(); //fork the programo
            if(pid==-1) {
                fprintf(stderr, "%sError: fork() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                continue;
            }
            if(pid==0) { //if in the child process...
                if(execvp(parse_array[0], parse_array)==-1) { //...execute the command! error if command does not exist
                    fprintf(stderr, "%sError: exec() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                    exit(EXIT_FAILURE); //exit the child process
                }
            }
            else { //otherwise, if in the parent process...
                wait(&stat); //wait for the child process to complete
                if(stat==-1) {
                    fprintf(stderr, "%sError: wait() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                    continue;
                }
                if(interrupt) { //idk how this works, got this from office hours lol
                    wait(NULL); //but this prevents the format from being messed up if a command is interrupted
                }
            }
        }
    }
    return 0;
}



int main(int argc, char** argv) {
    uid_t user=getuid();
    struct passwd* user_struct=getpwuid(user); //get current user
    if(user_struct==NULL) {
        fprintf(stderr, "%sError: Cannot get passwd entry. %s.%s\n", RED, strerror(errno), DEFAULT);
    }
    strcpy(homedir, user_struct->pw_dir);
    prompt(); //start the shell loop
    return 0;
}