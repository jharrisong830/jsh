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
#include <linux/limits.h>


char* parse_cd(int, char**, const char*);

int get_tokens(char**, int*);