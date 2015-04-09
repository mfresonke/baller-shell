#include "builtins.h"

struct Alias *aliases[MAX_ALIASES];

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

char* get_cd()
{
	//TODO make this better
	return strdup( getenv( "PWD" ) );
}