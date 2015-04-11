#include "commands.h"

//Define Varaiables

struct Command *command_start = NULL;
struct Command *command_end = NULL;
bool cmd_run_in_bkgrnd = false;
bool apply_output_append = false;
char *file_input = NULL;		
char *file_output_std = NULL;	
char *file_output_err = NULL;

void new_command( char *cmd_abs_path, char *invocation )
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
	add_arg( strdup( invocation ) );

	//set the path
	command_end->command_abs_path = cmd_abs_path;
}

void new_command_abs( char *cmd )
{
	new_command( cmd, cmd );
}
void new_command_relative( char *cmd )
{
	char *cmd_abs_path = path_rel_to_abs( cmd );
	new_command( cmd_abs_path, cmd );
}
void new_command_path( char *cmd )
{
	//search the path to see if command exists
	char *cmd_abs_path = search_path( cmd );

	//if the command exists, run it!
	if( cmd_abs_path )
	{
		new_command( cmd_abs_path, cmd );
	}
	else
	{
		//otherwise, throw an error.
		error_command_not_found( cmd );
	}
}

void run_commands()
{
	pid_t pid = -1;
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

		//Flush Buffer before forking!
		fflush(0);

		//Do The Forking!
		pid = fork();

		if( pid > 0 )		//if parent process
		{	
			//if we have not just forked the beginning command...
			if( command_curr != command_start )
			{
				close( pipes_recieve[1] );
				close( pipes_recieve[0] );
			}

			//advance the pointer
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
			/* Setup Input/Output Redirection if applicable */

			if ( command_curr == command_start )
				redirect_input_apply();

			if( command_curr == command_end )
				redirect_output_apply();


			//if only one command...
			if ( command_end == command_start )
			{
				//no command to command pipes to set up.
			}
			//if a set of piped commands...
			else if( command_curr == command_start )
			{
				// set standard out to pipe
				dup2( pipes_send[1], STDOUT_FILENO );
				close( pipes_send[0] );
			}
			else if( command_curr == command_end )
			{
				//only set standard in
				dup2( pipes_recieve[0], STDIN_FILENO );
				close( pipes_recieve[1] );
			}
			else //if in the middle of a chain of piped commands...
			{
				//set both standard in & out.
				dup2( pipes_recieve[0], STDIN_FILENO );
				dup2( pipes_send[1], STDOUT_FILENO );
				close( pipes_send[0] );
				close( pipes_recieve[1] );
			}
			int exit_code = execv(command_abs_path, command_args);
			printf( "Execution failed with exit code: %d\n", exit_code );
			break;
		}
		//Move the pipes over to set up for the next iteration
		pipes_recieve[0] = pipes_send[0];
		pipes_recieve[1] = pipes_send[1];
	}
	redirect_clear();

	//If the user does NOT want the command to run in the background...
	if ( cmd_run_in_bkgrnd )
	{
		cmd_run_in_bkgrnd = false;
	}
	else
	{
		//then wait on your children!
		waitpid( pid, NULL, 0 );
	}
	
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

char* search_path( char *cmd )
{
	//get path from enviornment
	char *path_raw = strdup( getenv( "PATH" ) );

	//check that path exists.
	if( !path_raw )
	{
		error_path_search("PATH Variable not found.");
		return NULL;
	}

	const char DELIMITER[2] = ":";

	//Used to separate different paths.
	char *dir = strtok( path_raw, DELIMITER );
	while( dir )
	{
		char *cmd_abs_path = search_dir( dir, cmd );
		//if the search did not return NULL...
		if ( cmd_abs_path )
		{
			//return an acceptable string.
			return cmd_abs_path;
		}
		dir = strtok( NULL, DELIMITER );
	}
	
	//if all else fails, return NULL!
	return NULL;
}

char* search_dir( char *abs_path, char *cmd )
{
	//Open Directory for Reading
	DIR *dir_ptr = opendir( abs_path );
	//Check that directory is valid
	if ( !dir_ptr )
	{
		return NULL;
	}

	//Start iterating through the directory
	struct dirent *dir_entry = readdir( dir_ptr );
	while ( dir_entry )	//while we still have files to iterate through...
	{
		char *filename = dir_entry->d_name;
		//if the filename matches...
		if ( strcmp( filename, cmd ) == 0 )
		{
			char *filename_abs = combine_dir_and_file( abs_path, filename );
			//...and the file is valid...
			if( is_file_valid( filename_abs ) )
			{
				//...we found the file!
				closedir( dir_ptr );
				return filename_abs;
			}
			else
			{
				free( filename_abs );
			}
		}
		//iterates to the next file in the directory
		dir_entry = readdir( dir_ptr );
	}

	closedir( dir_ptr );
	return NULL;
}


bool is_file_valid( char *filename )
{
	struct stat file_stat;

	if ( stat( filename, &file_stat ) < 0 )
	{
		return false;
	}

	//determine if a valid type
	if( S_ISREG( file_stat.st_mode ) ) //if it's just a regular file...
	{
		return true;
	}
	return false;
}

void redirect_input_setup( char *file )
{
	//Attempt to open the file to check if it exists/can be read.
	FILE *file_exists = fopen( file, "r" );
	if( file_exists )
	{
		file_input = file;
		fclose( file_exists );
		return;
	}
	//now that we know there are no errors, we set the global var.
	file_input = NULL;
	//TODO Check why (i.e. could be permission error)
	error_redirect_input( "Input file does not exist." );
	//return( ERROR );
}

void redirect_input_apply()
{
	//check if applicable
	if( !file_input )
	{
		//if null, return.
		return;
	}

	//Open the input as read-only
	int fd = open( file_input, O_RDONLY );

	//check for open error
	if( fd < 0 )
	{
		error_redirect_input( "Input file could not be opened." );
		return;
	}

	dup2( fd, STDIN_FILENO );
}

void redirect_output_append_setup( char *file )
{
	file_output_std = file;
	apply_output_append = true;
}

void redirect_output_overwrite_setup( char *file )
{
	file_output_std = file;
}

void redirect_output_apply()
{
	//check if applicable
	if( !file_output_std )
	{
		//if null, return.
		return;
	}

	int fd;

	//if output append was called
	if( apply_output_append )
	{
		//append to the file if no errors occur
		if( ( fd = open( file_output_std, O_WRONLY | O_APPEND | O_CREAT, 
			       S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ) == -1 ) 
		{
			error_redirect_output( "Output file does not exist." );
			return;
		}
	}
	//otherwise
	else
	{
		//overwrite the file if no errors occur
		if( ( fd = open(file_output_std, O_WRONLY | O_CREAT | O_TRUNC,
    	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ) == -1 )
    	{
    		error_redirect_output( "Output file does not exist." );
    		return;
    	}
	}

	dup2( fd, STDOUT_FILENO );

}

void redirect_clear()
{
	free( file_input );
	file_input = NULL;
	free( file_output_std );
	file_output_std = NULL;	
	free( file_output_err );
	file_output_err = NULL;
	apply_output_append = NULL;
}
