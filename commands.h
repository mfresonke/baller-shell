#ifndef B_SHELL_COMMANDS_H
#define B_SHELL_COMMANDS_H
#define MAX_ARGS 100

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

/* ============ Begin Command Structure ============ */

/** Runs all commands. */
void run_commands();

enum CommandType
{
	ABSOLUTE,
	RELATIVE,
	BUILTIN,
	PATH
};

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

/** Adds a new command to linked list. Automatically links previous Command. Uses the command type and command to set the command_path var correctly, and to fill in the first element of command_args. */
void new_command( enum CommandType type, char *command );


/* ============ Begin command_end Operations ============ */

/** Safely deletes all of the arguments and strings relating to the command. Leaves NULL Values. */
void clear_commands();
void clear_commands_helper( struct Command *curr );

/** Adds an argument to the command_args var. */
void add_arg( char *arg );

#endif