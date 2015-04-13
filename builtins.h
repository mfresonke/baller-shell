#ifndef B_SHELL_BUILTINS_H
#define B_SHELL_BUILTINS_H

#define MAX_PATH_LENGTH 200
#define MAX_ALIASES 200
#define ALIAS_NOT_FOUND -1
#define ALIAS_DELIMITERS " \"\n"
#define ENV_TOKEN_START "${"
#define ENV_TOKEN_END "}"
#define MAX_WILDCARD_RESULTS 200
#define MAX_LINE_SIZE 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "string_manipulation.h"
#include "errors.h"
#include <glob.h>
#include "shell.lex.h"

//system variable which stores the list of
//environment variables
extern char **environ;

/** Gets the current working directory. */
char* get_cd();

/* ========== CD ========== */
/** The main cd command can take in a relative path, or an absolute path. You must call its helper functions to take in anything else. */
void cd( char *dir );
/** Changes the current directory to home. */
void cd_home();
/** Changes the current directory based on a word instead of a path (e.g. 'output' ) */
void cd_word( char *word );

/* ========= Begin Environment Structure ======= */

/** Prints all of the environment variables in the form variable=value */
void env_print();
/** Adds the variable name to the environment with a value if the variable does not exist. If it exists, update
 **  variable with new value. */
void env_set( char *vname, char *value );
/** Unsets an environment variable by name so it no longer exists*/
void env_unset( char *vname );

/* ========= Begin Alias Structure ======= */

struct Alias 
{
	char *name;		//name of the alias
	char *command;	//command the alias represets!
};

extern struct Alias *aliases[MAX_ALIASES];
extern size_t alias_count;
extern bool *alias_used;

/** Prints all aliases! */
void alias_print();
void alias_set( char *name, char *command );
void alias_unset( char *name );

/** Searches for an alias of name "name". Returns index if found, ALIAS_NOT_FOUND if not found. */
int alias_search( char *name );

/* ========= Begin Wildcard Stucture ======= */

extern char *expansions[MAX_WILDCARD_RESULTS];
extern size_t expansion_count;

void expand_wildcard( char *input );

/* ========= Begin Pre-parse Structure ======= */

/** Preparses Input. Returns char of fixed input. */
char* run_preparser( char *input );

char* search_and_apply_aliases( char *input );
char* search_and_apply_env_vars( char *input );

#endif