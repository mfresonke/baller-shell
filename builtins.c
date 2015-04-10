#include "builtins.h"

struct Alias *aliases[MAX_ALIASES];
size_t alias_count = 0;

char* get_cd()
{
	//TODO make this better
	return strdup( getenv( "PWD" ) );
}

void cd( char *dir )
{
	//if dir is NULL (empty) then by default we go back to home.
	if( !dir || dir[0] == '\0' )
	{
		dir = getenv( "HOME" );
	}

	//change working directory
	if( chdir( dir ) < 0 )
	{
		error_directory_invalid();
		return;
	}

	char pwd[MAX_PATH_LENGTH];

	//change enviornment variables
	setenv( "OLDPWD", getenv("PWD"), 1 );
	getcwd( pwd, MAX_PATH_LENGTH );
	setenv( "PWD", pwd, 1 );
	
}

void cd_home()
{
	//Simply Call cd with NULL
	cd( NULL );
}

void cd_word( char *word )
{
	//transform the string into a relative path
	char* path_rel = word_to_rel_path( word );
	//call cd
	cd( path_rel );
}

void env_print()
{
	//duplicate system environ variable
	//for printing
	char **environ_ptr = environ;

	//if an environment variable exists...
	while( *environ_ptr )
	{
			//print out each variable and name
			printf( "%s\n", *environ_ptr );
			++environ_ptr;
	}
}

void env_set( char *vname, char *value )
{

	if( setenv( vname, value, 1 ) < 0 )
	{
		error_set_environment_variable( vname );
		return;
	}
}

void env_unset( char *vname )
{
	if( unsetenv( vname ) < 0 )
	{
		error_unset_environment_variable( vname );
		return;
	}
}

void alias_print()
{
	int a;
	for( a = 0; a != alias_count; ++a )
	{
		printf( "%s=%s\n", aliases[a]->name, aliases[a]->command );
	}
}

void alias_set( char *name, char *command )
{
	//search for if the alias already exists
	int alias_index = alias_search( name );
	if( alias_index != ALIAS_NOT_FOUND )
	{
		//if the alias already exists, just change the command.
		free( aliases[alias_index]->command );
		aliases[alias_index]->command = command;
		return;
	}

	//if the alias doesn't already exist, we need to make one.

	//create a new Alias struct and set its properties.
	struct Alias *alias_new = malloc( sizeof( struct Alias ) );
	alias_new->name = name;
	alias_new->command = command;

	//add it to the array
	aliases[alias_count] = alias_new;
	++alias_count;
}

void alias_unset( char *name )
{
	//search to see if the alias exists
	int alias_index = alias_search( name );

	if( alias_index != ALIAS_NOT_FOUND ) //if the alias exists...
	{
		//free alias properties
		free( aliases[alias_index]->name );
		free( aliases[alias_index]->command );

		//free alias itself
		free( aliases[alias_index] );
		aliases[alias_index] = NULL;

		//decrement alias count
		--alias_count;

		//shift the array
		int a;
		for( a = alias_index; a != alias_count; ++a )
		{
			aliases[a] = aliases[a+1];
		}
		//clean up the last pointer
		aliases[a] = NULL;
	}
	else //alias does not exist...
	{
		error_alias_not_exist( name );
	}
}

int alias_search( char *name )
{
	//check if there are any elements in our list
	if( alias_count == 0 )
	{
		return ALIAS_NOT_FOUND;
	}

	int a;
	//start searching
	for( a = 0; a != alias_count; ++a )
	{
		if( strcmp( aliases[a]->name, name ) == 0 ) //if we found the alias...
		{
			return a;
		}
	}

	//if nothing was found, return ALIAS_NOT_FOUND
	return ALIAS_NOT_FOUND;
}

char* apply_aliases( char *input_raw )
{
	int a;
	int b;
	char *input = input_raw;

	//make a bool array that will keep track if we used a particular alias (to detect recursion)
	bool alias_used[MAX_ALIASES];
	//make all needed values false
	for( a = 0; a != alias_count; ++a )
		alias_used[a] = false;

	//for each alias...
	for( a = 0; a != alias_count; ++a )
	{
		size_t input_length = strlen( input );
		char *alias_name = aliases[a]->name;
		size_t alias_length = strlen( alias_name );
		//used to account for possible spaces at the beginning of input.
		size_t offset = 0;
		//for each character in the input...
		for( b = 0; b != input_length; ++b )
		{
			size_t alias_index = ( b - offset );
			//check if current char is a space
			if( input[b] == ' ' )
			{
				//if so, add one to our offset.
				++offset;
				continue;
			}

			//if the beginning of user input does NOT match the alias name...
			if( input[b] != alias_name[alias_index] )
			{
				//break to the next alias to search
				break;
			}

			//if we are at the end of the alias_name...
			if( alias_index == ( alias_length-1 ) )
			{
				//then we have successfully found the alias! 

				//check if recursive loop
				if( alias_used[a] )
				{
					warn_alias_loop_detected();
					break;
				}
				
				//otherwise...

				//set it as used
				alias_used[a] = true;

				//Replace it with a command!
				char *input_new = replace_text( input, offset, b, aliases[a]->command );
				free( input );
				input = input_new;
			}
		}
	}
	//if our input has changed, we need to run this function again until it no longer detects any changes.
	if( input_raw != input )
	{
		return apply_aliases( input );
	}
	else
	{
		return input;
	}
}


