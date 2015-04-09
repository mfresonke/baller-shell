#ifndef B_SHELL_BUILTINS_H
#define B_SHELL_BUILTINS_H

#define MAX_PATH_LENGTH 200
#define MAX_ALIASES 100

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
	struct Alias *next;	//points to next alias
};

extern struct Alias *alias_start;
extern struct Alias *alias_end;

/** Prints all aliases! */
void alias_print();
void alias_set( char *name, char *command );
void alias_unset( char *name );

/** Searches for an alias of name "name". If found, returns the pointer to the alias with the matching name, and sets "prev" to the name of the previous alias. Returns NULL if not found. */
struct Alias* alias_search_with_prev( char *name, struct Alias **prev );

/** Searches for an alias of name "name". Returns NULL if not found. */
struct Alias* alias_search( char *name );

/** Frees a specific alias and its properties. */
void alias_free( struct Alias* alias );

#endif