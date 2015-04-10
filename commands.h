#ifndef B_SHELL_COMMANDS_H
#define B_SHELL_COMMANDS_H
#define MAX_ARGS 100
#define ERROR -34

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "errors.h"
#include "string_manipulation.h"

/* ============ Begin Command Structure ============ */

/** Runs all commands. */
void run_commands();

/** Determines if a set of commands will run in the background or foreground. */
extern bool cmd_run_in_bkgrnd;

/* Determines if output redirect is append or overwrite. */
extern bool apply_output_append;

/** SLL command structure. Has all the data needed for one command to run. */
struct Command
{
	char *command_abs_path;			//set to abs path of command to run
	char *command_args[MAX_ARGS];	//set to args of command to run
	size_t num_of_args;				//keeps track of number of command args
	struct Command *next;			//points to next command.				
};

//SLL Pointers
extern struct Command *command_start;
extern struct Command *command_end;

/* ============ Begin Command Creation/Deletion Operations ============ */

/** Adds a new command to linked list. Automatically links previous Command. Accepts only absoulte paths. Invocation is the string how the command was invoked. */
void new_command( char *cmd_abs_path, char *invocation );
void new_command_abs( char *cmd );
void new_command_relative( char *cmd );
void new_command_path( char *cmd );

/** Safely deletes all of the arguments and strings relating to the command. Leaves NULL Values. */
void clear_commands();
void clear_commands_helper( struct Command *curr );

/* ============ Begin command_end Operation ============ */

/** Adds an argument to the command_args var. */
void add_arg( char *arg );

/* ============ Begin PATH Search Operations ============ */

/** Searches path for 'cmd'. Returns abs dir if found, NULL if not found */
char* search_path( char *cmd );
/** Searches a specific directory for cmd. Returns full path if found, NULL if not */
char* search_dir( char *abs_path, char *cmd );
/** Returns true if a valid file, false, if not. */
bool is_file_valid( char *filename );


/* ============ Begin File Redirection Operations ============ */

//filenames for redirection. NULL if no redirection.
extern char *file_input;		
extern char *file_output_std;	
extern char *file_output_err;

/** Checks if the given file is viable for input redirection. If so, it sets the appropriate vars. To be used by parent process. */
void redirect_input_setup( char *file );
/** Applies input redirection, if applicable. To be called by child process. */
void redirect_input_apply();

/** Sets up a given file to be appended with new data and sets append output flag.  */
void redirect_output_append_setup( char *file );
/** Sets up a given file to be overwritten with new data */
void redirect_output_overwrite_setup( char *file );
/** Apply output redirection, if applicable. To be called by child procces. */
void redirect_output_apply();
/** Clears and Resets all redirect variables. */
void redirect_clear();

#endif