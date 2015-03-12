#include "builtins.h"

void cd( char *dir )
{
	//if dir is NULL (empty) then by default we go back to home.
	if( !dir )
	{
		dir = getenv( "HOME" );
	}
	//change enviornment variables
	setenv( "OLDPWD", getenv("PWD"), 1 );
	setenv( "PWD", dir, 1 );
	//change working directory
	chdir( dir );
}