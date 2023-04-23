/**
 * AUTHOR:  John Graham
 * 
 * FILE:    parse.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef __linux__
    #include <linux/limits.h>
#else
    #include <limits.h>
#endif


char* parse_cd(int, char**, const char*);

int get_tokens(char*, char**, int*);