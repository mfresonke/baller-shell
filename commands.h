#ifndef B_SHELL_COMMANDS_H
#define B_SHELL_COMMANDS_H
#define MAX_ARGS 100

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "errors.h"
#include "string_manipulation.h"
#include <errno.h>

/* ============ Begin Command Structure ============ */

/** Runs all commands. */
void run_commands();

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


/* ============ Begin command_end Operations ============ */

/** Safely deletes all of the arguments and strings relating to the command. Leaves NULL Values. */
void clear_commands();
void clear_commands_helper( struct Command *curr );

/** Adds an argument to the command_args var. */
void add_arg( char *arg );

/* ============ Begin PATH Setup Operations ============ */

/** Searches path for 'cmd'. Returns abs dir if found, NULL if not found */
char* search_path( char *cmd );
/** Searches a specific directory for cmd. Returns full path if found, NULL if not */
char* search_dir( char *abs_path, char *cmd );
/** Returns true if a valid file, false, if not. */
bool is_file_valid( char *filename );

#endif