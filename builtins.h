#ifndef B_SHELL_BUILTINS_H
#define B_SHELL_BUILTINS_H

#define MAX_PATH_LENGTH 200

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "string_manipulation.h"
#include "errors.h"

//system variable which stores the list of
//environment variables
extern char **environ;

/* ========== CD ========== */
/** The main cd command can take in a relative path, or an absolute path. You must call its helper functions to take in anything else. */
void cd( char *dir );
/** Changes the current directory to home. */
void cd_home();
/** Changes the current directory based on a word instead of a path (e.g. 'output' ) */
void cd_word( char *word );

/* ========= Begin Environment Structure =======*/

/** Prints all of the environment variables in the form variable=value */
void printenv();


char* get_cd();

#endif