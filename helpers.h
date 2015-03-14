#ifndef B_SHELL_HELPERS_H
#define B_SHELL_HELPERS_H
#define MAX_ARGS 100

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

enum CommandType
{
	ABSOLUTE,
	RELATIVE,
	BUILTIN,
	PATH
};

extern char *command_abs_path;			//set to abs path of command to run
extern char *command_args[MAX_ARGS];	//set to args of command to run
extern size_t num_of_args;				//keeps track of number of command args

/** Prints the prompt before a command is entered. */
void print_prompt();

/** Uses the command type and command to set the command_path var correctly, and to fill in the first element of command_args */
void set_command( enum CommandType type, char *command );

/** Runs an absolute command. Variable "command" and "command_args" should be set correctly before running. */
int run_command();

/** Safely deletes all of the arguments and strings relating to the command. Leaves NULL Values. */
void clear_command();

/** Adds an argument to the command_args var. */
void add_arg( char *arg );


#endif