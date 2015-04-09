#include "builtins.h"

struct Alias *alias_start = NULL;
struct Alias *alias_end = NULL;

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
	struct Alias *curr = alias_start;
	while( curr )
	{
		printf( "%s=%s\n", curr->name, curr->command );
		curr = curr->next;
	}
}

void alias_set( char *name, char *command )
{
	//search for if the alias already exists
	struct Alias *alias_existing = alias_search( name );
	if( alias_existing )
	{
		//if the alias already exists, just change the command.
		free( alias_existing->command );
		alias_existing->command = command;
		return;
	}

	//if the alias doesn't already exist, we need to make one.

	//create a new Alias struct and set its properties.
	struct Alias *alias_new = malloc( sizeof( struct Alias) );
	alias_new->name = name;
	alias_new->command = command;
	alias_new->next = NULL;

	//add it to the linked-list
	if( alias_end ) //if there are already items in the list...
	{
		alias_end->next = alias_new;
		alias_end = alias_new;
	}
	else //if there are no items in the list...
	{
		alias_start = alias_new;
		alias_end = alias_new;
	}
}

void alias_unset( char *name )
{
	//search to see if the alias exists
	struct Alias *alias_prev = NULL;
	struct Alias *alias_existing = alias_search_with_prev( name, &alias_prev );

	if( alias_existing ) //if the alias exists...
	{
		//if a previous exists,
		if( alias_prev )
		{
			//set the next pointer properly
			alias_prev->next = alias_existing->next;
		}

		//fix linked-list pointers.

		if( alias_existing == alias_start )
			alias_start = alias_existing->next;

		if( alias_existing == alias_end )
			alias_end = alias_prev;

		//free the alias
		alias_free( alias_existing );		
	}
	else //alias does not exist...
	{
		error_alias_not_exist( name );
	}
}

struct Alias* alias_search_with_prev( char *name, struct Alias **prev )
{
	//check if there are any elements in our list
	if( !alias_start || !alias_end )
	{
		//if not, return NULL
		return NULL;
	}

	//define variables for searching
	struct Alias *curr = alias_start;
	struct Alias *prev_temp = NULL;

	//start searching
	while( curr )
	{
		if( strcmp( curr->name, name ) == 0 ) //if we found the alias...
		{
			//set prev
			*prev = prev_temp;
			return curr;
		}
		//set variables for next iteration
		prev_temp = curr;
		curr = curr->next;
	}

	//if nothing was found, return NULL
	return NULL;
}

struct Alias* alias_search( char *name )
{
	struct Alias *dummy;
	return alias_search_with_prev( name, &dummy );
}

void alias_free( struct Alias* alias )
{
	//free alias properties
	free( alias->name );
	free( alias->command );

	//free alias itself
	free( alias );
}
