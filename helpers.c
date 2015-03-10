#include "helpers.h"

//Define Varaiables
char *command_abs_path = NULL;
char *command_args[MAX_ARGS] = { NULL };
size_t num_of_args = 0;
enum CommandType command_type;


void set_command( enum CommandType type, char *command )
{
	//set first argument of command args
	add_arg( strdup( command ) );

	//set command_abs_path to correct location
	switch( type )
	{
		case ABSOLUTE:
			//since path is already absolute, we do not have to do anything!
			command_abs_path = command;
			break;
		case RELATIVE:
		case BUILTIN:
		case PATH:
			printf( "THAT COMMAND TYPE IS NOT YET IMPLEMENTED.\n");
	}
}

int run_command() 
{
	printf( "Running Command %s !\n", command_args[0] );
	unsigned int status;

	if ( fork () == 0 )  
	{       /*  == 0 means in child  */
		int exit_code = execv(command_abs_path, command_args);	
		printf( "Execution failed with exit code: %d\n", exit_code );
    }
    else  
    {                      /* != 0 means in parent */
    	wait (&status);
    	//clear command data
    	clear_command();
    }

    return( status );
}

void add_arg( char *arg ) 
{
	//add string to array
	command_args[num_of_args] = arg;
	//increase number of elements
	++num_of_args;
	//make sure next element is NULL
	command_args[num_of_args] = NULL;
}

void clear_command() 
{
	//clear path
	free( command_abs_path );
	command_abs_path = NULL;

	//if array is already empty, return.
	if( !command_args[0] )
		return;

	//otherwise, loop through each element and delete the strings.
	char **str = command_args;
	while( *str != NULL )
	{
		free(*str);
		*str = NULL;
		++str;
	}
	num_of_args = 0;
}