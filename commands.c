#include "commands.h"

//Define Varaiables
enum ArgType command_type;
//SLL Pointers
struct Command *command_start = NULL;
struct Command *command_end = NULL;

void new_command( enum ArgType type, char *command )
{
	//Create new Command Struct
	struct Command *new_command = malloc( sizeof(struct Command) );
	//fill in default values.
	new_command->command_abs_path = NULL;
	new_command->command_args[0] = NULL;
	new_command->num_of_args = 0;
	new_command->next = NULL;

	//add it to the linked list.
	if( command_end ) //if list has items...
	{
		command_end->next = new_command;
		command_end = new_command;
	}
	else //if the linked list is currently empty.
	{
		command_start = new_command;
		command_end = new_command;
	}

	//set first argument of command args to how program was invoked
	add_arg( strdup( command ) );

	//set command_abs_path to correct location
	switch( type )
	{
		case ABSOLUTE:
			//since path is already absolute, we do not have to do anything!
			command_end->command_abs_path = command;
			break;
		case RELATIVE:
		case PATH:
			printf( "THAT COMMAND TYPE IS NOT YET IMPLEMENTED.\n");
	}
}

void run_commands()
{

	//Now we need to fork an arbitrary number of times.
	struct Command *command_curr = command_start;
	while( command_curr )
	{
		//Set Up Pipe Variables
		int pipes_recieve[2];
		int pipes_send[2];
		if( command_curr != command_end )
			pipe( pipes_send );

		//Set Up Info Needed For Exec
		char *command_abs_path = command_curr->command_abs_path;
		char **command_args = command_curr->command_args;

		//Do The Forking!
		pid_t pid = fork();

		if( pid > 0 )		//if parent process
		{
			command_curr = command_curr->next;
		}
		else if( pid < 0 )	//if parent and error forking
		{
			//TODO handle error case here
			printf( "Error forking process!" );
			break;
		}
		else if( pid == 0 )	//if child
		{
			//if only one command...
			if ( command_end == command_start )
			{
				//no command to command pipes to set up.
			}
			//if a set of piped commands...
			else if( command_curr == command_start )
			{
				// only set standard out
				dup2( pipes_send[1], STDOUT_FILENO );
			}
			else if( command_curr == command_end )
			{
				//only set standard in
				dup2( pipes_recieve[0], STDIN_FILENO );
			}
			else //if in the middle of a chain of piped commands...
			{
				//set both standard in & out.
				dup2( pipes_recieve[0], STDIN_FILENO );
				dup2( pipes_send[1], STDOUT_FILENO );
			}
			int exit_code = execv(command_abs_path, command_args);
			printf( "Execution failed with exit code: %d\n", exit_code );
			break;
		}
		//Move the pipes over to set up for the next iteration
		pipes_recieve[0] = pipes_send[0];
		pipes_recieve[1] = pipes_send[1];
	}
	//wait on the last process to finish!
	wait( NULL );
}

void add_arg( char *arg ) 
{
	//add string to array
	command_end->command_args[command_end->num_of_args] = arg;
	//increase number of elements
	++command_end->num_of_args;
	//make sure next element is NULL
	command_end->command_args[command_end->num_of_args] = NULL;
}

void clear_commands()
{
	clear_commands_helper( command_start );
	command_start = NULL;
	command_end = NULL;
}

void clear_commands_helper( struct Command *curr ) 
{
	//return if NULL
	if( !curr )
		return;

	//clear path
	free( curr->command_abs_path );
	curr->command_abs_path = NULL;

	//if array is already empty, return.
	if( !curr->command_args[0] )
		return;

	//otherwise, loop through each element and delete the strings.
	char **str = curr->command_args;
	while( *str != NULL )
	{
		free(*str);
		*str = NULL;
		++str;
	}
	curr->num_of_args = 0;

	//grab the pointer of the next command before destruction
	struct Command *next = curr->next;

	//now actually deallocate the struct itself
	free( curr );

	//recursively run this until all are deleted.
	clear_commands_helper( next );

}