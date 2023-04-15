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
#include <dirent.h>
#include <sys/stat.h>

#include "parse.h"


/*defining ANSI color sequences*/
#define BLUE    "\x1b[34;1m"
#define RED     "\x1b[31;1m"
#define GREEN   "\x1b[32;1m"
#define B_GREEN "\x1b[42;1m"
#define DEFAULT "\x1b[0m"

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
    char* parser; //holds each token from strtok
    char* parse_array[PATH_MAX]; //string array to hold each all of the tokens together
    pid_t pid; //holds the process ID number

    while(1) { //loop for the shell
        interrupt=0; //reset interrupt to false

        /* RECEIVING INPUT */
        printf("%s[%s]%s> ", BLUE, cwd, DEFAULT); //prints the shell in blue
        if(fgets(x, PATH_MAX, stdin)==NULL) { //read user input into x
            if(interrupt) { //dom't print error if interrupted by SIGINT
                interrupt=0;
            }
            else {
                fprintf(stderr, "%sError: Failed to read from stdin. %s.%s\n", RED, strerror(errno), DEFAULT);
            }
            continue;
        }

        /* PARSING INPUT */
        x[strcspn(x, "\n")]='\0'; //replace newline with null terminator
        int argc=0;
        parser=strtok(x, " "); //begin reading tokens, separating by space
        if(parser==NULL) {
            continue; //continue if no input supplied
        }
        while(parser!=NULL) { //while there are still tokens to be read...
            parse_array[argc]=parser; //add parsed result to the string array
            argc++; //increment argc
            parser=strtok(NULL, " "); //read the next token
        }
        parse_array[argc]=NULL; //add a null entry after the last token

        /* COMMANDS */
        if(strcmp(parse_array[0], "exit")==0) { //exit command
            free(cwd);
            exit(EXIT_SUCCESS); //exit the loop and end the program
        }
        else if(strcmp(parse_array[0], "cd")==0) { //cd command
            if(argc>2) { //error if too many arguments
                fprintf(stderr, "%sError: Too many arguments to cd.%s\n", RED, DEFAULT);
                continue;
            }
            if(argc==1 || strcmp(parse_array[1], "~")==0) { //if no other arguments supplied, or given '~'...
                if(chdir(homedir)!=0) { //change to the user's home directory
                    fprintf(stderr, "%sError: Cannot change directory to %s. %s.%s\n", RED, homedir, strerror(errno), DEFAULT);
                    continue;
                }
            }
            else if(chdir(parse_array[1])!=0) { //change to the specified directory
                fprintf(stderr, "%sError: Cannot change directory to %s. %s.%s\n", RED, parse_array[1], strerror(errno), DEFAULT);
                continue;
            }
            free(cwd); //free the old directory...
            if((cwd=getcwd(NULL, 0)) == NULL) { //...and set the new one
                fprintf(stderr, "%sError: Cannot get current working directory. %s.%s\n", RED, strerror(errno), DEFAULT);
                continue;
            }
        }
        else if(strcmp(parse_array[0], "ls")==0) { //EXTRA CREDIT -> colorized ls
            if(argc==1) { //if no other arguments supplied, print contents of current directory
                DIR* directory;
                if((directory=opendir(cwd)) == NULL) { //open directory
                    fprintf(stderr, "%sError: opendir() failed. %s.%s\n", RED, strerror(errno), DEFAULT);
                    continue;
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
                    DIR* directory;
                    if((directory=opendir(cwd)) == NULL) { //open directory
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



int main() {
    uid_t user=getuid();
    struct passwd* user_struct=getpwuid(user); //get current user
    if(user_struct==NULL) {
        fprintf(stderr, "%sError: Cannot get passwd entry. %s.%s\n", RED, strerror(errno), DEFAULT);
    }
    strcpy(homedir, user_struct->pw_dir);
    prompt(); //start the shell loop
    return 0;
}